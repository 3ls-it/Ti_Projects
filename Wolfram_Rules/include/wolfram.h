/*
 * @file    include/wolfram.h
 * @brief   Wolfram cellular automata rules.
 *
 * This file is part of Wolfram Rules.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

typedef unsigned char uchar;

typedef struct {
    uchar number;
    const char *name;
} WolframRule;

uchar wolfram_next(uchar rule,
                   uchar left,
                   uchar center,
                   uchar right);

extern const WolframRule wolfram_rules[];
extern const unsigned short wolfram_rule_count;

