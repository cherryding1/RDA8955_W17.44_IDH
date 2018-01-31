/*
 * drivers/clk/clkdev.c
 *
 *  Copyright (C) 2008 Russell King.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Helper for the clk API to assist looking up a struct clk.
 */
#include "list.h"
#include "clk.h"
#include "clkdev.h"

static LIST_HEAD(clocks);

/* It is thread safe as clks are registered only once in the beginning */
static HANDLE clocks_mutex;
#define mutex_lock(lock)
#define mutex_unlock(lock)

/*
 * Find the correct struct clk for the device.
 */
static struct clk_lookup *clk_find(const unsigned long cc)
{
    struct clk_lookup *p, *cl = NULL;

    list_for_each_entry(p, &clocks, node)
    {
        if (p->four_cc == cc)
        {
            cl = p;
            break;
        }
    }
    return cl;
}

struct clk *clk_get_sys(const unsigned long cc)
{
    struct clk_lookup *cl;

    mutex_lock(clocks_mutex);
    cl = clk_find(cc);
    if (cl && !__clk_get(cl->clk))
        cl = NULL;
    mutex_unlock(clocks_mutex);

    return cl ? cl->clk : NULL;
}

struct clk *clk_get(const unsigned long four_cc)
{
    return clk_get_sys(four_cc);
}

void clk_put(struct clk *clk)
{
    __clk_put(clk);
}

void clkdev_add(struct clk_lookup *cl)
{
    mutex_lock(clocks_mutex);
    list_add_tail(&cl->node, &clocks);
    mutex_unlock(clocks_mutex);
}

void clkdev_add_table(struct clk_lookup *cl, size_t num)
{
    mutex_lock(clocks_mutex);
    while (num--)
    {
        list_add_tail(&cl->node, &clocks);
        cl++;
    }
    mutex_unlock(clocks_mutex);
}
