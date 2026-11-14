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
//#define USE_TI92PLUS
//#define USE_V200

#define WORLD_WIDTH 159
#define WORLD_HEIGHT 99
#define WORLD_X_MAX (WORLD_WIDTH - 1)
#define WORLD_Y_MAX (WORLD_HEIGHT - 1)
#define MAX_CELLS 2000
#define NEIGHBOR_COUNT_MASK  0x0F
#define CURRENTLY_ALIVE_FLAG 0x80
#define HASH_SIZE 4096
#define HASH_MASK (HASH_SIZE - 1)


#include <args.h>
#include <estack.h>
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
#include "../include/p24_glidergun2022.h"


static WorldCell cells_a[MAX_CELLS];
static WorldCell cells_b[MAX_CELLS];

static unsigned char candidate_counts[MAX_CELLS];
static unsigned short last_candidate_count;
static unsigned short candidate_slots[HASH_SIZE];

static WorldCell *current_cells = cells_a;
static WorldCell *next_cells = cells_b;

static unsigned char cells_overflowed;
static unsigned short current_count;
static unsigned short next_count;

static const Cell *selected_pattern;
static unsigned short selected_pattern_count;
static unsigned char *display_name;
static unsigned short gens;



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


// Display Start screen
void
start_screen(void)
{
    draw_border();
    FontSetSys(F_8x10);
    DrawStr(27, 10, "Game of Life", A_REPLACE);
    return;
}


// Load a pattern to the World
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
draw_world(const WorldCell *cells,
           unsigned short count)
{
    unsigned short i;
    long x;
    long y;

    clrscr();

    for (i = 0; i < count; ++i)
    {
        x = cells[i].x;
        y = cells[i].y;

        if (x >= 0 && x <= WORLD_X_MAX &&
            y >= 0 && y <= WORLD_Y_MAX)
        {
            DrawPix((short)x, (short)y, A_NORMAL);
        }
    }
}


static unsigned short
hash_coordinate(long x, long y)
{
    unsigned long h;

    h = (unsigned long)x;
    h ^= (unsigned long)y
         + 0x9e3779b9UL
         + (h << 6)
         + (h >> 2);

    return (unsigned short)(h & HASH_MASK);
}


static void
clear_candidate_hash(void)
{
    unsigned short i;

    for (i = 0; i < HASH_SIZE; ++i)
        candidate_slots[i] = 0;
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


static short
find_candidate(long x, long y)
{
    unsigned short slot;
    unsigned short stored;
    short index;

    slot = hash_coordinate(x, y);

    for (;;)
    {
        stored = candidate_slots[slot];

        /*
         * Zero means that this hash slot has not been used.
         * Therefore, the coordinate is not present.
         */
        if (stored == 0)
            return -1;

        /*
         * Stored values are candidate_index + 1.
         */
        index = (short)(stored - 1);

        if (next_cells[index].x == x &&
            next_cells[index].y == y)
        {
            return index;
        }

        /*
         * Linear probing.
         */
        slot = (unsigned short)((slot + 1) & HASH_MASK);
    }
}


static short
ensure_candidate(long x, long y)
{
    unsigned short slot;
    short index;

    index = find_candidate(x, y);

    if (index >= 0)
        return index;

    if (next_count >= MAX_CELLS)
    {
        cells_overflowed = 1;
        return -1;
    }

    /*
     * Find an empty hash slot for the new coordinate.
     */
    slot = hash_coordinate(x, y);

    while (candidate_slots[slot] != 0)
        slot = (unsigned short)((slot + 1) & HASH_MASK);

    next_cells[next_count].x = x;
    next_cells[next_count].y = y;
    candidate_counts[next_count] = 0;

    /*
     * Store candidate index + 1 because zero means empty.
     */
    candidate_slots[slot] =
        (unsigned short)(next_count + 1);

    ++next_count;

    return (short)(next_count - 1);
}


static void
mark_current_cell(long x,
                  long y)
{
    short index;
    index = ensure_candidate(x, y);

    if (index >= 0)
        candidate_counts[index] |= CURRENTLY_ALIVE_FLAG;
}


static void
add_neighbor(long x, long y)
{
    short index;
    index = ensure_candidate(x, y);

    if (index >= 0)
		candidate_counts[index] =
			(candidate_counts[index] & CURRENTLY_ALIVE_FLAG) |
			((candidate_counts[index] & NEIGHBOR_COUNT_MASK) + 1);
}


static void
calculate_next_generation(void)
{
    unsigned short i;
    unsigned short candidate_count;
    short dx;
    short dy;
    long x;
    long y;

	unsigned char count;
	unsigned char alive;

    clear_candidate_hash();

    next_count = 0;
    cells_overflowed = 0;

    /*
     * Build the candidate list and neighbor counts.
     */
    for (i = 0; i < current_count; ++i)
    {
        x = current_cells[i].x;
        y = current_cells[i].y;

		/*
		 * The live cell itself is a candidate, but does not
		 * contribute to its own neighbor count.
		 */
		mark_current_cell(x, y);

		for (dy = -1; dy <= 1; ++dy)
		{
			for (dx = -1; dx <= 1; ++dx)
			{
				if (dx != 0 || dy != 0)
					add_neighbor(x + dx, y + dy);
			}
		}

    }

    if (cells_overflowed)
        return;

    /*
     * Save the number of candidates before compacting
     * next_cells[] in place.
     */
    candidate_count = next_count;
    last_candidate_count = candidate_count;
    next_count = 0;

    /*
     * Apply the Life rules and compact the survivors/births
     * into the beginning of next_cells[].
     */

	for (i = 0; i < candidate_count; ++i)
	{
		x = next_cells[i].x;
		y = next_cells[i].y;

		count = candidate_counts[i] & NEIGHBOR_COUNT_MASK;
		alive = candidate_counts[i] & CURRENTLY_ALIVE_FLAG;

		if (count == 3 ||
			(count == 2 && alive))
		{
			next_cells[next_count].x = x;
			next_cells[next_count].y = y;
			++next_count;
		}
	}
}


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
        selected_pattern_count = P80_GLIDERLESS_HWSS_COUNT;
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
		display_name = "Snark Loop";
    }
	else if (strcmp(name, "p24_gg_2022") == 0)
    {
        selected_pattern = p24glidergun2022;
        selected_pattern_count = P24GLIDERGUN2022_COUNT;
		display_name = "Period 24 Glider Gun, 2022";
    }

}



// Entry point
void
_main(void)
{
    select_pattern();
    gens = 128;

    if (selected_pattern_count > MAX_CELLS)
    {
        printf_xy(2, 2, "Pattern too large");
        ngetchx();
        exit(0);
    }

    clrscr();
    start_screen();
    FontSetSys(F_6x8);
    printf_xy(5, 40, "Pattern:");
    printf_xy(5, 50, display_name);
    FontSetSys(F_4x6);
    printf_xy(5, 80, "(Press any key to continue)");
    ngetchx();

	load_pattern_to_world_cells(selected_pattern,
                                selected_pattern_count,
                                current_cells);

	current_count = selected_pattern_count;

	draw_world(current_cells, current_count);

	{
		WorldCell *temp_cells;
		unsigned short step;

		for (step = 0; step < gens; ++step)
		{
			unsigned short live_before;
			unsigned short live_after;
			live_before = current_count;

			calculate_next_generation();

			if (cells_overflowed)
			{
				printf_xy(2, 2, "Candidate limit exceeded");
				printf_xy(2, 12, "Gen: %u", step);

				ngetchx();
				exit(0);
			}

			live_after = next_count;
			draw_world(next_cells, next_count);
#if 0
			printf_xy(2, 2, "Live bef: %u  ", current_count);
			printf_xy(2, 12, "#Cands: %u  ", last_candidate_count);
			printf_xy(2, 22, "Live aft:  %u   ", live_after);
			printf_xy(150, 2, "%u", step + 1);
			ngetchx();
#endif
			//draw_world(next_cells, next_count);
			current_count = next_count;

			temp_cells = current_cells;
			current_cells = next_cells;
			next_cells = temp_cells;
		}
	}
    DrawStr(2, 2, "Done", A_REPLACE);
	printf_xy(2, 12, "Gen: %u", gens);
	ngetchx();
	exit(0);
}
