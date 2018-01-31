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

#ifndef __CLOCK_8955_H
#define __CLOCK_8955_H

#include "list.h"
#include "clkdev.h"
#include "cs_types.h"

#define EINVAL 22
#define BIT(nr) (1UL << (nr))

struct pll_data
{
    uint32_t phys_base;
    uint32_t num;
    uint32_t flags;
    uint32_t input_rate;
    uint32_t div_ratio_mask;
};
#define PLL_HAS_PREDIV 0x01
#define PLL_HAS_POSTDIV 0x02

struct gate_data
{
    uint32_t reg_en;
    uint32_t val_en;
    uint32_t reg_dis;
    uint32_t val_dis;
};

#define DIV_LOOKUP_TABLE 0x01
#define DIV_TRIGGER 0x02
#define DIV_DENOM_ONLY 0x04
struct div_data
{
    uint32_t reg;
    uint32_t mask;
    uint8_t trig_bit;
    uint8_t shift;
    uint8_t flags;
    uint8_t reserved;
};

struct clk
{
    struct list_head node;
    unsigned long four_cc;

    unsigned long rate;
    unsigned long req_par_rate;
    unsigned long minrate; /* H/W supported max rate */
    unsigned long maxrate; /* H/W supported max rate */
    uint8_t usecount;
    uint32_t flags;
    struct clk *parent;
    struct clk *fast_parent;
    struct clk *slow_parent;
    struct list_head children;  /* list of children */
    struct list_head childnode; /* parent's child list node */
    struct pll_data *pll_data;
    struct gate_data *gate_data;
    struct div_data *div_data;
    unsigned long (*recalc)(struct clk *);
    unsigned long (*sysclk_cb)(unsigned long rate);
    int (*set_rate)(struct clk *clk, unsigned long rate);
    int (*round_rate)(struct clk *clk, unsigned long rate);
    int (*check_parent)(struct clk *clk);
};

#define CLK_FIXED BIT(1)        /* clk rate is constant */
#define CLK_PLL BIT(2)          /* PLL-derived clock */
#define CLK_SYS BIT(3)          /* SYS-derived clock */
#define GATE_MANUAL BIT(4)      /* default is auto */
#define PRE_PLL BIT(5)          /* source is before PLL mult/div */
#define IMPLICIT_ENABLED BIT(6) /* enabled implicitly */
#define RATE_REQUEST BIT(7)     /* request specific rate */

#define CLK(cc, ck)    \
    {                  \
        .four_cc = cc, \
        .clk = ck,     \
    }

int rda_clk_init(struct clk_lookup *clocks);
int rda_set_sysclk_rate(struct clk *clk, unsigned long rate);
int rda_set_pllclk_rate(struct clk *clk, unsigned long rate);
int rda_clk_check_parent(struct clk *clk);

void rda_ck_show(bool verbose);

int clk_enable(struct clk *clk);
void clk_disable(struct clk *clk);
unsigned long clk_get_rate(struct clk *clk);
long clk_round_rate(struct clk *clk, unsigned long rate);
int clk_set_rate(struct clk *clk, unsigned long rate);
int clk_register(struct clk *clk);
void clk_unregister(struct clk *clk);

#endif
