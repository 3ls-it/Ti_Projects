/*
* @file    ant.c
* @brief   Langton's Ant cellular automaton
*
* Copyright (c) 2026 J Adams <jfa63@duck.com>
*
* SPDX-License-Identifier: BSD-2-Clause
*/

#define SAVE_SCREEN
#define MIN_AMS 100
#define USE_TI89

#include <graph.h>
#include <math.h>
#include <kbd.h>
#include <stdbool.h>
#include <stdio.h>
#include <tigcclib.h>

SCR_RECT screen_area={{0,0,159,99}};



// draw_border() written by Zeljko Juric for Cave Blaster
void
draw_border(void)
{
    int i;
    ClrScr();
    for(i=0;i<=3;i+=2)
        DrawClipRect(MakeWinRect(i,i,159-i,99-i),&screen_area,A_NORMAL);
}


int
start_screen(void)
{
    char buf[16];
    int len = 0;
    short key;
    short num;
    buf[0] = '\0';

    draw_border();
    FontSetSys(F_8x10);
    DrawStr(25, 10, "Langton's Ant", A_REPLACE);
    FontSetSys(F_4x6);
    printf_xy(5, 30, "Number of steps: ");

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
        DrawStr(30, 10, "Langton's Ant", A_REPLACE);
        FontSetSys(F_4x6);
        printf_xy(5, 30, "Number of steps: %s", buf);

    }

    num = atol(buf);
    if (num <= 0 || num > 32767)
    {
        return 32767;
    } else return num;
}



void
_main(void)
{
    clrscr();
    int nsteps = start_screen();
    draw_border();
    FontSetSys(F_4x6);
    printf_xy(5, 30, "Running %i steps", nsteps);
	printf_xy(5, 40, "(Press any key to continue)");
    ngetchx();
    clrscr();

    // initial values and limits
    short dir = 0;
    short yp = 49;
    short xp = 79;
    short yh = 99;
    short xh = 158;

    DrawPix(xp, yp, A_NORMAL);    
    yp -= 1;

    int step = 0;
    while(step <= nsteps && step < 32767)
    {
        // If pixel on, turn anti-clockwise, then turn pixel off
        if (GetPix(xp, yp))
        {
            DrawPix(xp, yp, A_REVERSE);
            dir = (dir - 1) % 4;
            if (dir == -1) dir = 3;

        } else if (!GetPix(xp, yp)) { // if off, turn clockwise, then turn on
 
            DrawPix(xp, yp, A_NORMAL);    
            dir = (dir + 1) % 4;
        }

        // Move one cell in current dir
        if (dir == 0)
        {
            yp -= 1;
        } else if (dir == 1) {
            xp += 1;
        } else if (dir == 2) {
            yp += 1;
        } else if (dir == 3) {
            xp -= 1;
        }

        // Allow screeen wrapping
        if (xp > xh) 
        {
            xp = 0;
        } else if (xp < 0) {
            xp = xh;
        }

        if (yp > yh) {
            yp = 0;
        } else if (yp < 0) {
            yp = yh;
        }
        step += 1;
    }
    ngetchx(); //Pause before clearing
    exit(0);
}
