/*
 *  include/linux/clkdev.h
 *
 *  Copyright (C) 2008 Russell King.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Helper for the clk API to assist looking up a struct clk.
 */
#ifndef __CLKDEV_H
#define __CLKDEV_H

#include <cs_types.h>

struct clk;

#define __clk_get(clk) ({ 1; })
#define __clk_put(clk) \
    do                 \
    {                  \
    } while (0)

struct clk_lookup
{
    struct list_head node;
    unsigned long four_cc;
    struct clk *clk;
};

void clkdev_add(struct clk_lookup *cl);
void clkdev_drop(struct clk_lookup *cl);

void clkdev_add_table(struct clk_lookup *, size_t);

#endif
