/*
 * @file    src/wrules.c
 * @brief   Wolfram cellular automata rules.
 *
 * This file is part of Wolfram Rules.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#define SAVE_SCREEN
#define MIN_AMS 100
#define USE_TI89
//#define USE_TI92PLUS
//#define USE_V200

#define WORLD_WIDTH 159
#define WORLD_HEIGHT 99
#define WORLD_X_MAX (WORLD_WIDTH - 1)
#define WORLD_Y_MAX (WORLD_HEIGHT - 1)

#define HORIZONTAL_MARGIN (WORLD_HEIGHT - 1)
#define SIM_WIDTH (WORLD_WIDTH + (2 * HORIZONTAL_MARGIN))
#define SIM_X_MAX (SIM_WIDTH - 1)
#define VIEW_OFFSET HORIZONTAL_MARGIN

#include <args.h>
#include <graph.h>
#include <kbd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/wolfram.h"

static uchar ROW[SIM_WIDTH];
static uchar NEW[SIM_WIDTH];

const WolframRule wolfram_rules[] = {
    { 22,  "Rule 22"  },
    { 30,  "Rule 30"  },
    { 90,  "Rule 90"  },
    { 110, "Rule 110" }
};

const unsigned short wolfram_rule_count =
    sizeof(wolfram_rules) / sizeof(wolfram_rules[0]);



// draw_border() written by Zeljko Juric for Cave Blaster
void
draw_border(void)
{
    SCR_RECT screen_area={{0,0,159,99}};

	int i;
	clrscr();
	for(i=0;i<=3;i+=2)
		DrawClipRect(MakeWinRect(i,i,159-i,99-i),&screen_area,A_NORMAL);
}


// Display Start screen
uchar
start_screen(void)
{
	char buf[16];
	buf[0] = '\0';
	short len = 0;
	short key;
	uchar num;

	draw_border();
	FontSetSys(F_8x10);
	DrawStr(27, 10, "Wolfram Rules", A_REPLACE);
	FontSetSys(F_4x6);
	printf_xy(5, 30, "Rule number (e.g. 110): ");

	while (1)
	{
		key = ngetchx();

		if (key == KEY_ENTER)
			break;

		if (key == KEY_CLEAR && len > 0)
		{
			len--;
			buf[len] = '\0';
		}
		else if (key >= '0' && key <= '9' && len < 15)
		{
			buf[len++] = (char)key;
			buf[len] = '\0';
		}

		clrscr();
		draw_border();
		FontSetSys(F_8x10);
		DrawStr(27, 10, "Wolfram Rules", A_REPLACE);
		FontSetSys(F_4x6);
		printf_xy(5, 30, "Rule number (e.g. 110): %s", buf);
	}

	num = (uchar) atoi(buf);
	if (num <= 0 || num > 255)
	{
		return 30;
	} else return num;
}


// Apply rule
uchar
wolfram_next(uchar rule,
			 uchar left,
			 uchar center,
			 uchar right)
{
    uchar neighborhood;
	neighborhood = (left << 2) |
				   (center << 1) |
				   right;

	return (rule >> neighborhood) & 1;
}



// Entry point
void
_main(void)
{
	uchar rule_num; 
	uchar p,q,r,i;
	short x,y,ic;

	memset(ROW, 0, sizeof ROW);
	memset(NEW, 0, sizeof NEW);

	//Set rule here
	rule_num = start_screen();

	FontSetSys(F_6x8);
	printf_xy(5, 40, "Rule:");
	printf_xy(5, 50, "%u", rule_num);
	FontSetSys(F_4x6);
	printf_xy(5, 80, "(Press any key to continue)");
	ngetchx();
	clrscr();

	// Set initial condition with one cell
	if (rule_num == 110)
	{
		ic = WORLD_X_MAX;
	}
	else ic = 77;

	ROW[VIEW_OFFSET + ic] = 1;
	DrawPix(ic, 0, A_NORMAL);

	for (y = 1; y <= WORLD_Y_MAX; ++y)
	{
		for (x = 0; x < SIM_WIDTH; ++x)
		{
			if (x == 0) p = 0;
			else p = ROW[x - 1];

			q = ROW[x];

			if (x == SIM_X_MAX)
				r = 0;
			else
				r = ROW[x + 1];

			i = wolfram_next(rule_num, p, q, r);

			NEW[x] = i ? 1 : 0;

			//Draw only the visible viewport.
			if (x >= VIEW_OFFSET &&
				x < VIEW_OFFSET + WORLD_WIDTH &&
				i)
			{
				DrawPix(x - VIEW_OFFSET, y, A_NORMAL);
			}
		}

		memcpy(ROW, NEW, sizeof ROW);
		memset(NEW, 0, sizeof NEW);
	}

	ngetchx();
	return;
}
