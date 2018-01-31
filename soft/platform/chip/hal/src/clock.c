/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */

#include "hal_clk.h"
#include "clock_8955.h"
#include "sys_ctrl.h"
#include "global_macros.h"
#include "reg_alias.h"

#include "hal_sys.h"
#include "halp_debug.h"
#include "halp_sys.h"

static LIST_HEAD(g_clocks);

//#define mutex_lock(lock) COS_LockMutex(lock)
//#define mutex_unlock(lock) COS_UnlockMutex(lock)

#define reg_write(reg, val) (*(volatile unsigned long *)(reg) = (val))
#define reg_read(reg) (*(volatile unsigned long *)(reg))
#define reg_or(reg, val) reg_write((reg), (reg_read(reg) | (val)))
#define max(a, b) (((a) > (b)) ? (a) : (b))

#ifndef WARN_ON
#define __WARN() HAL_TRACE(0, 0, "warning on line:%d", __LINE__);
#define WARN_ON(condition) ({          \
    int __ret_warn_on = !!(condition); \
    if (__ret_warn_on)                 \
        __WARN();                      \
    (__ret_warn_on);                   \
})
#endif

const u32 clk_rate_lookup_table[] = {
#ifdef CHIP_DIE_8909
    /* mapping 0-5 to min rate 56M */
    HAL_CLK_RATE_56M, HAL_CLK_RATE_56M, HAL_CLK_RATE_56M,
    HAL_CLK_RATE_56M, HAL_CLK_RATE_56M, HAL_CLK_RATE_56M,
    HAL_CLK_RATE_61M, HAL_CLK_RATE_67M, HAL_CLK_RATE_75M,
    HAL_CLK_RATE_84M, HAL_CLK_RATE_96M, HAL_CLK_RATE_112M,
    HAL_CLK_RATE_134M, HAL_CLK_RATE_168M, HAL_CLK_RATE_224M,
    HAL_CLK_RATE_336M,
#else
    HAL_CLK_RATE_26M, HAL_CLK_RATE_39M, HAL_CLK_RATE_52M,
    HAL_CLK_RATE_78M, HAL_CLK_RATE_89M, HAL_CLK_RATE_104M,
    HAL_CLK_RATE_113M, HAL_CLK_RATE_125M, HAL_CLK_RATE_139M,
    HAL_CLK_RATE_156M, HAL_CLK_RATE_178M, HAL_CLK_RATE_208M,
    HAL_CLK_RATE_250M, HAL_CLK_RATE_312M,
#endif
};

static u32 sys_clk_rate_to_idx(unsigned long rate);
static void propagate_rate(struct clk *root);

static void inline sys_ctrl_unlock() { hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK; }

static void inline sys_ctrl_lock() { hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK; }

static void clk_gate_config(struct gate_data *gd, bool enable)
{
    if (!gd)
        return;

    sys_ctrl_unlock();
    if (enable)
        reg_write(gd->reg_en, gd->val_en);
    else
        reg_write(gd->reg_dis, gd->val_dis);
    sys_ctrl_lock();
}

static void __clk_enable(struct clk *clk)
{
    if (clk->parent)
        __clk_enable(clk->parent);
    if (clk->usecount++ == 0 && (clk->flags & GATE_MANUAL))
        clk_gate_config(clk->gate_data, true);
    if (clk->recalc)
        clk->rate = clk->recalc(clk);
}

static void __clk_disable(struct clk *clk)
{
    struct clk *parent;
    if (WARN_ON(clk->usecount == 0))
        return;
    if (--clk->usecount == 0 && (clk->flags & GATE_MANUAL))
        clk_gate_config(clk->gate_data, false);

    parent = clk->parent;
    if (parent)
    {
        __clk_disable(parent);
        if (clk->flags & CLK_SYS)
        {
            if (WARN_ON(clk->req_par_rate > parent->rate))
                return;
            if (clk->req_par_rate == parent->rate)
            {
                struct clk *clk_t;
                unsigned long max = parent->minrate;
                list_for_each_entry(clk_t, &parent->children, childnode)
                {
                    if (clk_t->flags & RATE_REQUEST &&
                        clk_t->usecount > 0 &&
                        clk_t->req_par_rate > max)
                        max = clk_t->req_par_rate;
                }
                if (max != parent->rate)
                {
                    parent->set_rate(parent, max);
                    propagate_rate(parent);
                }
                clk->req_par_rate = parent->minrate;
            }
        }
    }
}

int clk_enable(struct clk *clk)
{
    unsigned long flags;

    if (clk == NULL)
        return -EINVAL;

    flags = hal_SysEnterCriticalSection();
    __clk_enable(clk);
    hal_SysExitCriticalSection(flags);
    return 0;
}

void clk_disable(struct clk *clk)
{
    unsigned long flags;

    if (clk == NULL)
        return;

    flags = hal_SysEnterCriticalSection();
    __clk_disable(clk);
    hal_SysExitCriticalSection(flags);
}

unsigned char clk_get_usecount(struct clk *clk)
{
    if (clk == NULL)
        return 0;

    return clk->usecount;
}

unsigned long clk_get_rate(struct clk *clk)
{
    if (clk == NULL)
        return -EINVAL;

    return clk->rate;
}

long clk_round_rate(struct clk *clk, unsigned long rate)
{
    if (clk == NULL)
        return -EINVAL;

    if (clk->round_rate)
        return clk->round_rate(clk, rate);

    return clk->rate;
}

/* Propagate rate to children */
static void propagate_rate(struct clk *root)
{
    struct clk *clk;

    list_for_each_entry(clk, &root->children, childnode)
    {
        if (clk->recalc && (clk->usecount > 0 || (clk->flags & IMPLICIT_ENABLED)))
            clk->rate = clk->recalc(clk);
        propagate_rate(clk);
    }
}

int clk_set_rate(struct clk *clk, unsigned long rate)
{
    unsigned long flags;
    int ret = -EINVAL;

    if (clk == NULL)
        return ret;

    if (clk->flags & CLK_FIXED)
        return 0;

    flags = hal_SysEnterCriticalSection();
    if (clk->set_rate)
        ret = clk->set_rate(clk, rate);

    if (ret == 0)
    {
        if (clk->recalc)
            clk->rate = clk->recalc(clk);
        propagate_rate(clk);
    }
    hal_SysExitCriticalSection(flags);
    return ret;
}

int clk_register(struct clk *clk)
{
    if (clk == NULL)
        return -EINVAL;

    INIT_LIST_HEAD(&clk->children);

    list_add_tail(&clk->node, &g_clocks);
    if (clk->parent)
        list_add_tail(&clk->childnode, &clk->parent->children);

    /* If rate is already set, use it */
    if (clk->rate)
        return 0;

    /* Else, see if there is a way to calculate it */
    if (clk->recalc)
        clk->rate = clk->recalc(clk);

    /* Otherwise, default to parent rate */
    else if (clk->parent)
        clk->rate = clk->parent->rate;

    return 0;
}

void clk_unregister(struct clk *clk)
{
    if (clk == NULL)
        return;

    list_del(&clk->node);
    list_del(&clk->childnode);
}

static unsigned long clk_sysclk_recalc(struct clk *clk)
{
    if (WARN_ON(!clk->div_data))
        return -EINVAL;

    unsigned long rate =
        clk_rate_lookup_table[reg_read(clk->div_data->reg)];

    return rate;
}

#ifdef CHIP_DIE_8909
static u32 sys_clk_rate_to_idx(unsigned long rate)
{
    if (rate < HAL_CLK_RATE_56M)
        return SYS_CTRL_SYS_FREQ_56MHZ;

    if (rate > HAL_CLK_RATE_336M)
        return SYS_CTRL_SYS_FREQ_336MHZ;

    switch (rate)
    {
    case HAL_CLK_RATE_56M:
        return SYS_CTRL_SYS_FREQ_56MHZ;
    case HAL_CLK_RATE_61M:
        return SYS_CTRL_SYS_FREQ_61MHZ;
    case HAL_CLK_RATE_67M:
        return SYS_CTRL_SYS_FREQ_67MHZ;
    case HAL_CLK_RATE_75M:
        return SYS_CTRL_SYS_FREQ_75MHZ;
    case HAL_CLK_RATE_84M:
        return SYS_CTRL_SYS_FREQ_84MHZ;
    case HAL_CLK_RATE_96M:
        return SYS_CTRL_SYS_FREQ_96MHZ;
    case HAL_CLK_RATE_112M:
        return SYS_CTRL_SYS_FREQ_112MHZ;
    case HAL_CLK_RATE_134M:
        return SYS_CTRL_SYS_FREQ_134MHZ;
    case HAL_CLK_RATE_168M:
        return SYS_CTRL_SYS_FREQ_168MHZ;
    case HAL_CLK_RATE_224M:
        return SYS_CTRL_SYS_FREQ_224MHZ;
    case HAL_CLK_RATE_336M:
        return SYS_CTRL_SYS_FREQ_336MHZ;
    default:
        return SYS_CTRL_SYS_FREQ_56MHZ;
    };
}
#else
static u32 sys_clk_rate_to_idx(unsigned long rate)
{
    switch (rate)
    {
    case HAL_CLK_RATE_26M:
        return SYS_CTRL_SYS_FREQ_26M;
    case HAL_CLK_RATE_39M:
        return SYS_CTRL_SYS_FREQ_39M;
    case HAL_CLK_RATE_52M:
        return SYS_CTRL_SYS_FREQ_52M;
    case HAL_CLK_RATE_78M:
        return SYS_CTRL_SYS_FREQ_78M;
    case HAL_CLK_RATE_89M:
        return SYS_CTRL_SYS_FREQ_89M;
    case HAL_CLK_RATE_104M:
        return SYS_CTRL_SYS_FREQ_104M;
    case HAL_CLK_RATE_113M:
        return SYS_CTRL_SYS_FREQ_113M;
    case HAL_CLK_RATE_125M:
        return SYS_CTRL_SYS_FREQ_125M;
    case HAL_CLK_RATE_139M:
        return SYS_CTRL_SYS_FREQ_139M;
    case HAL_CLK_RATE_156M:
        return SYS_CTRL_SYS_FREQ_156M;
    case HAL_CLK_RATE_178M:
        return SYS_CTRL_SYS_FREQ_178M;
    case HAL_CLK_RATE_208M:
        return SYS_CTRL_SYS_FREQ_208M;
    case HAL_CLK_RATE_250M:
        return SYS_CTRL_SYS_FREQ_250M;
    case HAL_CLK_RATE_312M:
        return SYS_CTRL_SYS_FREQ_312M;
    default:
        asm("break 1");
        return SYS_CTRL_SYS_FREQ_26M;
    };
}
#endif

int rda_set_pllclk_rate(struct clk *clk, unsigned long rate)
{
    /* If this is the PLL base clock, wrong function to call */
    if (clk->pll_data)
        return -EINVAL;

    /* There must be a parent... */
    if (WARN_ON(!clk->parent))
        return -EINVAL;

#if 0
    /* ... the parent must be a PLL... */
    if (WARN_ON(!clk->parent->pll_data))
        return -EINVAL;
#endif

    /* ... and this clock must have a divider. */
    if (WARN_ON(!clk->div_data))
        return -EINVAL;

    if (clk->rate == rate)
        return 0;

#ifdef CHIP_DIE_8809E2
#include "pmd_m.h"
    unsigned long old = clk->rate;
    if (clk->four_cc == FOURCC_SYSTEM)
    {
        if (old <= HAL_CLK_RATE_139M && rate > HAL_CLK_RATE_139M)
            pmd_SetCoreVoltage(PMD_CORE_VOLTAGE_USER_SYS, PMD_CORE_VOLTAGE_MEDIUM);
        if (old <= HAL_CLK_RATE_156M && rate > HAL_CLK_RATE_156M ||
            old <= HAL_CLK_RATE_104M && rate > HAL_CLK_RATE_104M)
            pmd_SetCoreVoltage(PMD_CORE_VOLTAGE_USER_SYS, PMD_CORE_VOLTAGE_HIGH);
    }
#endif

    sys_ctrl_unlock();
    if (clk->div_data->flags & DIV_LOOKUP_TABLE)
    {
        uint32_t mask = clk->div_data->mask;
        uint32_t reg_val = reg_read(clk->div_data->reg) & ~mask;

        reg_write(clk->div_data->reg,
                reg_val | (mask & (sys_clk_rate_to_idx(rate) << clk->div_data->shift)));
    }
    else if (rate > clk->minrate && clk->div_data->flags & DIV_DENOM_ONLY)
    {
        uint32_t denom = clk->parent->rate / rate;
        reg_write(clk->div_data->reg, clk->div_data->mask & denom);
    }
    sys_ctrl_lock();

#ifdef CHIP_DIE_8809E2
    if (clk->four_cc == FOURCC_SYSTEM)
    {
        if (old > HAL_CLK_RATE_104M && rate <= HAL_CLK_RATE_104M)
            pmd_SetCoreVoltage(PMD_CORE_VOLTAGE_USER_SYS, PMD_CORE_VOLTAGE_LOW);
    }
#endif

    clk->rate = rate;
    if (clk->check_parent)
        clk->check_parent(clk);

    if (clk->four_cc == FOURCC_SYSTEM)
    {
        g_halSysSystemFreq = rate;
#ifdef CHIP_XTAL_CLK_IS_52M
        if (rate > HAL_CLK_RATE_52M)
            hal_SysRequestDdrHighFreq(TRUE);
#endif
    }

    return 0;
}

int rda_set_sysclk_rate(struct clk *clk, unsigned long rate)
{
    /* There must be a parent... */
    if (WARN_ON(!clk->parent))
        return -EINVAL;

    if (WARN_ON(!(clk->flags & CLK_SYS)))
        return -EINVAL;

    if (clk->req_par_rate == rate)
        return 0;

    if (clk->parent->rate < rate)
    {
        clk->parent->set_rate(clk->parent, rate);
        propagate_rate(clk->parent);
    }
    else if (clk->req_par_rate == clk->parent->rate)
    {
        unsigned long max = clk->parent->minrate;
        struct clk *clk_tmp;
        list_for_each_entry(clk_tmp, &clk->parent->children, childnode)
        {
            if ((clk_tmp->flags & RATE_REQUEST) &&
                clk_tmp->usecount > 0 &&
                clk_tmp->req_par_rate > max &&
                clk_tmp != clk)
                max = clk_tmp->req_par_rate;
        }

        if (rate > max)
            max = rate;

        if (max != clk->parent->rate)
        {
            clk->parent->set_rate(clk->parent, max);
            propagate_rate(clk->parent);
        }
    }

    clk->req_par_rate = rate;
    if (clk->sysclk_cb)
        clk->rate = clk->sysclk_cb(clk->parent->rate);
    else
        clk->rate = clk->parent->rate;
    return 0;
}

static int __clk_set_parent(struct clk *clk, struct clk *parent)
{
    if (clk == NULL)
        return -EINVAL;

    if (clk->parent == NULL)
        return -EINVAL;

    HAL_ASSERT((clk->parent->usecount >= clk->usecount),
               "usecount err %d %d", clk->parent->usecount, clk->usecount);

    clk->parent->usecount -= clk->usecount;
    clk->parent = parent;
    list_del_init(&clk->childnode);
    list_add(&clk->childnode, &clk->parent->children);
    parent->usecount += clk->usecount;

    return 0;
}

int rda_clk_check_parent(struct clk *clk)
{
    int ret;
    if (clk->rate <= clk->minrate)
    {
        if (clk->parent == clk->slow_parent)
            return 0;
        ret = __clk_set_parent(clk, clk->slow_parent);
    }
    else
    {
        if (clk->parent == clk->fast_parent)
            return 0;
        ret = __clk_set_parent(clk, clk->fast_parent);
    }
    return ret;
}

static unsigned long clk_leafclk_recalc(struct clk *clk)
{
    if (WARN_ON(!clk->parent))
        return clk->rate;

    // sys clk's children
    if ((clk->flags & CLK_SYS) && clk->sysclk_cb)
        clk->rate = clk->sysclk_cb(clk->parent->rate);

    return clk->rate;
}

static unsigned long clk_pllclk_recalc(struct clk *clk)
{
    /* PLL rate is a constant */

    return clk->rate;
}

int rda_clk_init(struct clk_lookup *clocks)
{
    struct clk_lookup *c;
    struct clk *clk;
    size_t num_clocks = 0;

    for (c = clocks; c->clk; c++)
    {
        clk = c->clk;

        if (!clk->recalc)
        {

            /* Check if clock is a PLL */
            if (clk->pll_data)
                clk->recalc = clk_pllclk_recalc;

#if 0
            /* Else, if it is a PLL-derived clock */
            else if (clk->flags & CLK_PLL)
                clk->recalc = clk_sysclk_recalc;
#endif

            /* Otherwise, it is a leaf clock */
            else if (clk->parent && clk->sysclk_cb)
                clk->recalc = clk_leafclk_recalc;
        }

        // TODO: pll related
        if (clk->pll_data)
        {
            struct pll_data *pll = clk->pll_data;
        }

        /*
        if (clk->recalc)
            clk->rate = clk->recalc(clk);
        */

        clk_register(clk);
        num_clocks++;
    }

    clkdev_add_table(clocks, num_clocks);

    return 0;
}

/* SW sys clock consumer handling */
#define MAX_SW_CLK_USER_NUM 16
struct sw_clk_consumer
{
    struct list_head node;
    unsigned long four_cc;
    unsigned long req_rate;
};

struct sw_clk_consumer g_consumers[MAX_SW_CLK_USER_NUM];
static LIST_HEAD(g_sw_consumers);
static unsigned char g_consumer_num = 0;
extern struct clk *g_clk_sw;

int clk_request(const unsigned long four_cc, unsigned long rate)
{
    int i, ret = 0;
    struct sw_clk_consumer *scc;

    unsigned long flags = hal_SysEnterCriticalSection();

    /* check if the node exists and find the max rate */
    unsigned long curr_rate = g_clk_sw->req_par_rate;
    unsigned long max_rate = rate;
    bool exist = false;
    list_for_each_entry(scc, &g_sw_consumers, node)
    {
        if (scc->four_cc == four_cc)
        {
            scc->req_rate = rate;
            exist = true;
        }
        if (scc->req_rate > max_rate)
            max_rate = scc->req_rate;
    }

    /* found it, not a new user */
    if (exist) {
        if (max_rate != curr_rate)
            ret = g_clk_sw->set_rate(g_clk_sw, max_rate);
        hal_SysExitCriticalSection(flags);
        return ret;
    }

    /* new user, find an empty node and save it */
    for (i = 0; i < MAX_SW_CLK_USER_NUM; i++)
        if (g_consumers[i].four_cc == FOURCC_UNKNOWN)
            break;

    HAL_ASSERT((i < MAX_SW_CLK_USER_NUM), "Too many clock users!");
    g_consumers[i].four_cc = four_cc;
    g_consumers[i].req_rate = rate;
    list_add_tail(&g_consumers[i].node, &g_sw_consumers);

    /* enable it only once */
    if (clk_get_usecount(g_clk_sw) == 0)
        __clk_enable(g_clk_sw);

    if (max_rate != curr_rate)
        ret = g_clk_sw->set_rate(g_clk_sw, max_rate);

    hal_SysExitCriticalSection(flags);
    return ret;
}

int clk_release(const unsigned long four_cc)
{
    int ret = 0;
    struct sw_clk_consumer *scc, *tmp;

    unsigned long flags = hal_SysEnterCriticalSection();

    unsigned long curr_rate = g_clk_sw->req_par_rate;
    unsigned long next_rate = 0;
    bool exist = false;
    list_for_each_entry_safe(scc, tmp, &g_sw_consumers, node)
    {
        if (scc->four_cc == four_cc)
        {
            exist = true;
            scc->four_cc = FOURCC_UNKNOWN;
            list_del(&scc->node);
        }
        else if (scc->req_rate > next_rate)
        {
            next_rate = scc->req_rate;
        }
    }

    if (WARN_ON(!exist))
    {
        hal_SysExitCriticalSection(flags);
        return -EINVAL;
    }

    if (list_empty(&g_sw_consumers))
    {
        __clk_disable(g_clk_sw);
        g_clk_sw->req_par_rate = 0;
        hal_SysExitCriticalSection(flags);
        return 0;
    }

    if (next_rate != curr_rate)
        ret = g_clk_sw->set_rate(g_clk_sw, next_rate);

    hal_SysExitCriticalSection(flags);
    return ret;
}

unsigned long sw_clk_get_rate(const unsigned long cc)
{
    struct sw_clk_consumer *scc;
    unsigned long rate = HAL_CLK_RATE_32K; /* match CSW logic */
    unsigned long flags = hal_SysEnterCriticalSection();
    list_for_each_entry(scc, &g_sw_consumers, node)
    {
        if (scc->four_cc == cc)
        {
            rate = scc->req_rate;
            break;
        }
    }
    hal_SysExitCriticalSection(flags);
    return rate;
}

bool clk_is_active(const unsigned long cc)
{
    struct sw_clk_consumer *scc;
    unsigned long flags = hal_SysEnterCriticalSection();
    list_for_each_entry(scc, &g_sw_consumers, node)
    {
        if (scc->four_cc == cc)
        {
            hal_SysExitCriticalSection(flags);
            return true;
        }
    }
    hal_SysExitCriticalSection(flags);

    return false;
}

/* Debug dumping all clks */

static inline void cc2string(unsigned long cc, uint8_t s[])
{
    s[0] = (uint8_t)cc;
    s[1] = (uint8_t)(cc >> 8);
    s[2] = (uint8_t)(cc >> 16);
    s[3] = (uint8_t)(cc >> 24);
    s[4] = 0;
}

static void dump_clock(struct clk *parent, bool verbose, int level)
{
    struct clk *clk;
    uint8_t name[5];

    if (parent->usecount == 0 && !verbose)
        return;

    cc2string(parent->four_cc, name);

    hal_DbgTrace(0, (1 << 1), "CLK%d: %s users=%2d %9ld Hz\n",
                 level, name, parent->usecount, clk_get_rate(parent));
    /* REVISIT show device associations too */

    /* cost is now small, but not linear... */
    list_for_each_entry(clk, &parent->children, childnode)
    {
        dump_clock(clk, verbose, level + 1);
    }
}

static void dump_sw_clock(void)
{
    int i;
    uint8_t name[5];

    for (i = 0; i < MAX_SW_CLK_USER_NUM; i++)
    {
        if (g_consumers[i].four_cc == FOURCC_UNKNOWN)
            continue;

        cc2string(g_consumers[i].four_cc, name);

        hal_DbgTrace(0, (1 << 0), "SWCLK: %s %9ld Hz\n",
                     name, g_consumers[i].req_rate);
    }
}

void rda_ck_show(bool verbose)
{
    struct clk *clk;

    hal_DbgTrace(0, 0, "------------- CLOCK TREE -------------\n");
    list_for_each_entry(clk, &g_clocks, node)
    {
        if (!clk->parent)
            dump_clock(clk, verbose, 1);
    }

    dump_sw_clock();

    return 0;
}
