/*
* @file    src/gol2.c
* @brief   John Conway's Game of Life cellular automata
*
* Copyright (c) 2026 J Adams <jfa63@duck.com>
*
* SPDX-License-Identifier: BSD-2-Clause
*/

#define SAVE_SCREEN
#define MIN_AMS 100
#define USE_TI89
#define USE_TI92PLUS
#define USE_V200

#define WORLD_WIDTH 159
#define WORLD_HEIGHT 99
#define WORLD_X_MAX (WORLD_WIDTH - 1)
#define WORLD_Y_MAX (WORLD_HEIGHT - 1)
#define MAX_CELLS 2000
#define DEFAULT_STEPS 2400

#include <args.h>
#include <graph.h>
#include <kbd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Pattern definitions
#include "../include/gol_types.h"
#include "../include/blinker.h"
#include "../include/gosper_glider_gun.h"
#include "../include/max.h"
#include "../include/10-engine_corder_ship.h"
#include "../include/p80_gliderless_hwss.h"
#include "../include/wing_spaceship.h"
#include "../include/p43_gliderloop.h"


static WorldCell cells_a[MAX_CELLS];
static WorldCell cells_b[MAX_CELLS];

static WorldCell *current_cells = cells_a;
static WorldCell *next_cells = cells_b;

static unsigned char cells_overflowed;
static unsigned short current_count;
static unsigned short next_count;

static const Cell *selected_pattern;
static unsigned short selected_pattern_count;
static unsigned char *display_name;



// draw_border() written by Zeljko Juric for Cave Blaster
void
draw_border(void)
{
    SCR_RECT screen_area={{0,0,159,99}};

	int i;
    ClrScr();
    for(i=0;i<=3;i+=2)
        DrawClipRect(MakeWinRect(i,i,159-i,99-i),&screen_area,A_NORMAL);
}


void
start_screen(void)
{
    draw_border();
    FontSetSys(F_8x10);
    DrawStr(27, 10, "Game of Life", A_REPLACE);
    return;
}


// A small wraper for GetPix()
short
pn(short x, short y)
{
    if (GetPix(x, y))
        return 1;
    return 0;
}


// Load a pattern to the "World"
static void
load_pattern_to_world_cells(const Cell *pattern,
                            unsigned short pattern_count,
                            WorldCell *cells)
{
    unsigned short i;

    for (i = 0; i < pattern_count; ++i)
    {
        cells[i].x = (long)pattern[i].x;
        cells[i].y = (long)pattern[i].y;
    }
}


// Draw the world at current generation
static void
draw_world(const unsigned char buffer[][WORLD_WIDTH])
{
    short x;
    short y;

    clrscr();

    for (y = 0; y < WORLD_HEIGHT; ++y)
    {
        for (x = 0; x < WORLD_WIDTH; ++x)
        {
            if (buffer[y][x])
                DrawPix(x, y, A_NORMAL);
        }
    }
}


static void
update_world_display(
    const unsigned char current[][WORLD_WIDTH],
    const unsigned char next[][WORLD_WIDTH])
{
    short x;
    short y;

    for (y = 0; y < WORLD_HEIGHT; ++y)
    {
        for (x = 0; x < WORLD_WIDTH; ++x)
        {
            if (current[y][x] != next[y][x])
            {
                if (next[y][x])
                    DrawPix(x, y, A_NORMAL);
                else
                    DrawPix(x, y, A_REVERSE);
            }
        }
    }
}


// Count neighbors
short
pixel_count(short x, short y)
{
    short cnt = \
        pn(x-1, y-1) + \
        pn(x, y-1) + \
        pn(x+1, y-1) + \
        pn(x-1, y) + \
        pn(x+1, y) + \
        pn(x-1, y+1) + \
        pn(x, y+1) + \
        pn(x+1, y+1);
    return cnt;
}


void
load_pattern(const Cell *cells, unsigned short count)
{
    unsigned short i;
    clrscr();

    for (i = 0; i < count; ++i)
        DrawPix(cells[i].x, cells[i].y, A_NORMAL);
}


void
draw_cells(const Cell *cells, unsigned short count)
{
    unsigned short i;

    for (i = 0; i < count; ++i)
        DrawPix(cells[i].x, cells[i].y, A_NORMAL);
}

#if 0
static void
calculate_next_generation(
    const unsigned char current[][WORLD_WIDTH],
    unsigned char next[][WORLD_WIDTH])
{
    short x;
    short y;
    unsigned char neighbors;
    const unsigned char *row_current;
    unsigned char *row_next;
    next_count = 0;
	cells_overflowed = 0;

    for (y = 1; y < WORLD_Y_MAX; ++y)
    {
        row_current = current[y];
        row_next = next[y];

        for (x = 1; x < WORLD_X_MAX; ++x)
        {
            neighbors = neighbor_counts[y][x];

            if (neighbors == 3 ||
                (neighbors == 2 && row_current[x]))
            {
                row_next[x] = 1;

                if (next_count < MAX_CELLS)
                {
                    next_cells[next_count].x = x;
                    next_cells[next_count].y = y;
                    ++next_count;
                }
				else
				{
					cells_overflowed = 1;
				}
            }
            else
            {
                row_next[x] = 0;
            }
        }
    }
}
#endif

// Load pattern given as arg
static void
select_pattern(void)
{
    ESI argptr;
    const char *name;

    /* Default pattern when no argument is supplied. */
    selected_pattern = blinker;
    selected_pattern_count = BLINKER_COUNT;
	display_name = "Blinker";

    InitArgPtr(argptr);

    if (GetArgType(argptr) != STR_TAG)
        return;

	/* Register available patterns */
    name = GetStrnArg(argptr);

    if (strcmp(name, "max") == 0)
    {
        selected_pattern = max;
        selected_pattern_count = MAX_COUNT;
		display_name = "Max";
    }
	else if (strcmp(name, "gosper_glider_gun") == 0)
    {
        selected_pattern = gosper_glider_gun;
        selected_pattern_count = GOSPER_GLIDER_GUN_COUNT;
		display_name = "Gosper Glider Gun";
    }
	else if (strcmp(name, "10engine_corder_ship") == 0)
    {
        selected_pattern = _10_engine_corder_ship;
        selected_pattern_count = _10_ENGINE_CORDER_SHIP_COUNT;
		display_name = "10 Engine Corder Ship";
    }
	else if (strcmp(name, "p80_gliderless_hwss") == 0)
    {
        selected_pattern = p80_gliderless_hwss;
        selected_pattern_count =P80_GLIDERLESS_HWSS_COUNT;
		display_name = "P80 Gliderless HWSS Gun";
    }
	else if (strcmp(name, "wing_spaceship") == 0)
    {
        selected_pattern = wing_spaceship;
        selected_pattern_count = WING_SPACESHIP_COUNT;
		display_name = "Wing Spaceship";
    }
	else if (strcmp(name, "snark_loop") == 0)
    {
        selected_pattern = snark_loop;
        selected_pattern_count = SNARK_LOOP_COUNT;
		display_name = "Snark Loop, period 43";
    }
}


// Entry point
void
_main(void)
{
    select_pattern();

    if (selected_pattern_count > MAX_CELLS)
    {
        printf_xy(2, 2, "Pattern too large");
        ngetchx();
        exit(0);
    }

    load_pattern_to_world_cells(selected_pattern,
                                selected_pattern_count,
                                current_cells);

    current_count = selected_pattern_count;

	clrscr();
    printf_xy(2, 2, "Cells: %u", current_count);
    printf_xy(2, 12, "X: %ld", current_cells[0].x);
    printf_xy(2, 22, "Y: %ld", current_cells[0].y);

    ngetchx();
    exit(0);
}
