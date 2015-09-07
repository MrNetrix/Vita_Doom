// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// $Log:$
//
// DESCRIPTION:
//	DOOM graphics stuff for SDL library
//
//-----------------------------------------------------------------------------

static const char
rcsid[] = "$Id: i_x.c,v 1.6 1997/02/03 22:45:10 b1 Exp $";

#include <stdlib.h>

#include "m_swap.h"
#include "doomstat.h"
#include "i_system.h"
#include "v_video.h"
#include "m_argv.h"
#include "d_main.h"
#include "i_video.h"

#include "doomdef.h"

#include "PSP.h"


#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)
#define PIXEL_SIZE (4) /* change this if you change to another screenmode */
#define FRAME_SIZE (BUF_WIDTH * SCR_HEIGHT * PIXEL_SIZE)
#define ZBUF_SIZE (BUF_WIDTH SCR_HEIGHT * 2) /* zbuffer seems to be 16-bit? */
#define CKEY_MOVE 300
#define CKEY_WEAPON 301
#define MAX_CONTROL 10

int d_controls[MAX_CONTROL];
int controls_set = 0;
int d_clist_start = 0;
int d_clist_curpos = 0;
boolean analog = 0;

extern SceCtrlData ctl;

static unsigned int __attribute__((aligned(16))) list[262144];

// Fake mouse handling.
boolean		grabMouse;

// Blocky mode,
// replace each 320x200 pixel with multiply*multiply pixels.
// According to Dave Taylor, it still is a bonehead thing
// to use ....
static int	multiply=1;


unsigned int __attribute__((aligned(16))) colors[256];
 
/* fuck pspupdates.com */

#define REPEAT_TIME 0x40000
static unsigned long control_bef_ctl  = 0;
static unsigned long control_bef_tick = 0;

weapontype_t num;
static player_t*	plyrweap; 

struct Vertex
{
	float u,v;
	float x,y,z;
};

void I_ShutdownGraphics(void)
{
   
}



//
// I_StartFrame
//
void I_StartFrame (void)
{
     

}

void SetupControls(void)
{
/*  Example config.ini:
      ANALOG   = WEAPON
      START    = ENTER
      SELECT   = ESC
      CROSS    = USE
      DPAD     = MOVE
      TRIANGLE = MAP
      SQUARE   = RUN
      CIRCLE   = FIRE
      L_TRIG   = M_LEFT
      R_TRIG   = M_RIGHT
*/

    FILE *handle;
    int length;

    char config_file[270];
    char file_data[500];
    int i;
    int x, cur = 0;
    char type[50];
    char value[50];
    extern char *doomwaddir;

    // Default Controls if config.ini was not found
    d_controls[0]  = CKEY_WEAPON;
    d_controls[1]  = KEY_ENTER;
    d_controls[2]  = KEY_ESCAPE;
    d_controls[3]  = ' ';
    d_controls[4]  = CKEY_MOVE;
    d_controls[5]  = KEY_TAB;
    d_controls[6]  = KEY_RSHIFT;
    d_controls[7]  = KEY_RCTRL;
    d_controls[8]  = ',';
    d_controls[9]  = '.';
    d_controls[10] = 222;
    //scePowerSetClockFrequency(222,222,111);
    
    controls_set = 1;

    sprintf(config_file, "%sconfig.ini", doomwaddir);

    handle = fopen (config_file, "r");

    if(handle == NULL)
        return;

    // Needed since fscanf doesn't link...
    fseek(handle, 0, SEEK_END);
    length = ftell(handle);
    rewind(handle);

    if(length <= 500)
    {
	    memset(file_data, 0, sizeof(file_data));
	    fread (file_data, 1,length, handle);
    }
    else
        return;

	fclose(handle);

	
	while(cur==0 || file_data[cur])
	{
		//This could all be done with fscanf(handle, "%s = %s", type, value);
		for(x=0;file_data[cur] != ' ';x++)
		{
			type[x] = file_data[cur];
			cur++;
		}
		type[x] = 0;

		while(file_data[cur] == ' ' || file_data[cur] == '=')
			cur++;

		for(x=0;file_data[cur] != '\n';x++)
		{
			value[x] = file_data[cur];
			cur++;
		}
				
		if (value[x-1] == 13) {
		    value[--x] = 0;
		}
		else {
		    value[x] = 0;
		}
		
	    cur++;
		//We now have the values
        i = -1;
	
	if(!strcmp(type,"ANALOG"))          i = 0;
        else if(!strcmp(type,"START"))      i = 1;
        else if(!strcmp(type,"SELECT"))     i = 2;
        else if(!strcmp(type,"CROSS"))      i = 3;
        else if(!strcmp(type,"DPAD"))       i = 4;
        else if(!strcmp(type,"TRIANGLE"))   i = 5;
        else if(!strcmp(type,"SQUARE"))     i = 6;
        else if(!strcmp(type,"CIRCLE"))     i = 7;
        else if(!strcmp(type,"L_TRIG"))     i = 8;
        else if(!strcmp(type,"R_TRIG"))     i = 9;
	else if(!strcmp(type,"CPU_CLOCK"))  i = 10;

        if(i != -1)
        {
            if(!strcmp(value,"MOVE")) {
	        if (i == 0) {
	            analog = 1;
		    d_controls[i] = CKEY_MOVE;
		}
		else if (i == 7) {
		    analog = 0;
		    d_controls[i] = CKEY_MOVE;
		}
		else {
		    analog = 0;
		    d_controls[i] = CKEY_MOVE;
		}
	    }
	    else if(!strcmp(value,"WEAPON")) {
	        if (i == 0) {
	            analog = 0;
		    d_controls[i] = CKEY_WEAPON;
		}
		else if (i == 7) {
		    analog = 1;
		    d_controls[i] = CKEY_WEAPON;
		}
		else {
		    analog = 1;
		    d_controls[i] = CKEY_WEAPON;
		}
	    }
	    else if (!strcmp(value,"222")) {
	    	if (i == 10) {
		    d_controls[i] = 222;
		    //scePowerSetClockFrequency(222,222,111);
		}
	    }
	    else if (!strcmp(value,"266")) {
	    	if (i == 10) {
		    d_controls[i] = 266;
		    //scePowerSetClockFrequency(266,266,133);
		}
	    }
	    else if (!strcmp(value,"333")) {
	    	if (i == 10) {
		    d_controls[i] = 333;
		    //scePowerSetClockFrequency(333,333,166);
		}
	    }
            else if(!strcmp(value,"ENTER"))     d_controls[i] = KEY_ENTER;
            else if(!strcmp(value,"ESC"))       d_controls[i] = KEY_ESCAPE;
            else if(!strcmp(value,"USE"))       d_controls[i] = ' ';
            else if(!strcmp(value,"MAP"))       d_controls[i] = KEY_TAB;
            else if(!strcmp(value,"M_LEFT"))    d_controls[i] = ',';
            else if(!strcmp(value,"M_RIGHT"))   d_controls[i] = '.';
            else if(!strcmp(value,"RUN"))       d_controls[i] = KEY_RSHIFT;
            else if(!strcmp(value,"FIRE"))      d_controls[i] = KEY_RCTRL;
        }

    }
    
}



unsigned long Read_Key(void) {

    sceCtrlPeekBufferPositive(0, &ctl, 1);

	if (ctl.buttons == control_bef_ctl) {
		return 0;
	}
	control_bef_ctl  = ctl.buttons;
	return control_bef_ctl;
}

unsigned long Read_Key3(void) {

    sceCtrlPeekBufferPositive(0, &ctl, 1);

    if (ctl.ly >= 0xD0) ctl.buttons |= PSP2_CTRL_DOWN;  // DOWN
    if (ctl.ly <= 0x30) ctl.buttons |= PSP2_CTRL_UP;    // UP
    if (ctl.lx <= 0x30) ctl.buttons |= PSP2_CTRL_LEFT;  // LEFT
    if (ctl.lx >= 0xD0) ctl.buttons |= PSP2_CTRL_RIGHT; // RIGHT 

    if (ctl.buttons == control_bef_ctl) {
		return 0;
	}
    control_bef_ctl = ctl.buttons;
	return control_bef_ctl;
}

/* This processes SDL events */
void I_GetEvent()
{
    static  event_t  event;
    static  event_t  kbevent;

	unsigned long key;
	if(!controls_set)
        SetupControls();

	event.type = ev_joystick;
	event.data1 = 0;
	event.data2 = 0;
	event.data3 = 0;

	key = Read_Key();

	if (analog) {
 	    if (ctl.ly >= 0xD0) event.data3 = 1;  // DOWN
 	    if (ctl.ly <= 0x10) event.data3 = -1;    // UP
 	    if (ctl.lx <= 0x10) event.data2 = -1;  // LEFT
 	    if (ctl.lx >= 0xD0) event.data2 = 1;  // RIGHT
	}
    else {
        if (ctl.buttons & PSP2_CTRL_DOWN)	event.data3 = 1;  // DOWN
        if (ctl.buttons & PSP2_CTRL_UP) 	event.data3 = -1;    // UP
        if (ctl.buttons & PSP2_CTRL_LEFT)	event.data2 = -1;  // LEFT
        if (ctl.buttons & PSP2_CTRL_RIGHT)	event.data2 = 1;  // RIGHT
    }

    //printf("I_GetEvent: buttons: 0x%x data1: %d data2: %d data3: %d", ctl.buttons, event.data1, event.data2, event.data3);

	D_PostEvent(&event);
	
    if (key & PSP2_CTRL_LTRIGGER) {
        if (key & PSP2_CTRL_RTRIGGER) {
            if (key & PSP2_CTRL_START) {
	            NewPrompter();
	            I_FinishUpdate2();
	        }
	    }
	}
	else {
	
	if (d_controls[1] == KEY_ENTER || d_controls[1] == KEY_ESCAPE ||
	    d_controls[1] == ' ' || d_controls[1] == KEY_TAB) {
	    if (key & PSP2_CTRL_START)
	    {
	    kbevent.type = ev_keydown;
	    kbevent.data1 = d_controls[1];
	    D_PostEvent(&kbevent);
	    }
	    else  
	    {
	    kbevent.type = ev_keyup;
	    kbevent.data1 = d_controls[1];
	    D_PostEvent(&kbevent);
	    }  
	}
	else {
	    if (ctl.buttons & PSP2_CTRL_START)
	    {
	    kbevent.type = ev_keydown;
	    kbevent.data1 = d_controls[1];
	    D_PostEvent(&kbevent);
	    }
	    else  
	    {
	    kbevent.type = ev_keyup;
	    kbevent.data1 = d_controls[1];
	    D_PostEvent(&kbevent);
	    }
	}
	if (d_controls[2] == KEY_ENTER || d_controls[2] == KEY_ESCAPE ||
	    d_controls[2] == ' ' || d_controls[2] == KEY_TAB) {
 	    if (key & PSP2_CTRL_SELECT)
	    {
	    kbevent.type = ev_keydown;
	    kbevent.data1 = d_controls[2];
	    D_PostEvent(&kbevent);
	    }
	    else  
	    {
	    kbevent.type = ev_keyup;
	    kbevent.data1 = d_controls[2];
	    D_PostEvent(&kbevent);
	    }  
	}
	else {
	    if (ctl.buttons & PSP2_CTRL_SELECT)
	    {
	    kbevent.type = ev_keydown;
	    kbevent.data1 = d_controls[2];
	    D_PostEvent(&kbevent);
	    }
	    else  
	    {
	    kbevent.type = ev_keyup;
	    kbevent.data1 = d_controls[2];
	    D_PostEvent(&kbevent);
	    }
	}
	if (d_controls[3] == KEY_ENTER || d_controls[3] == KEY_ESCAPE ||
	    d_controls[3] == ' ' || d_controls[3] == KEY_TAB) {
	    if (key & PSP2_CTRL_CROSS)
	    {
	    kbevent.type = ev_keydown;
	    kbevent.data1 = d_controls[3];
	    D_PostEvent(&kbevent);
	    }
	    else
	    {
	    kbevent.type = ev_keyup;
	    kbevent.data1 = d_controls[3];
	    D_PostEvent(&kbevent);
	    }
	}
	else {
	    if (ctl.buttons & PSP2_CTRL_CROSS)
	    {
	    kbevent.type = ev_keydown;
	    kbevent.data1 = d_controls[3];
	    D_PostEvent(&kbevent);
	    }
	    else
	    {
	    kbevent.type = ev_keyup;
	    kbevent.data1 = d_controls[3];
	    D_PostEvent(&kbevent);
	    }
	}

	if (gamestate == GS_LEVEL)
	{
        boolean looped = false;

		plyrweap = &players[consoleplayer];

		num = plyrweap->readyweapon;
		
		if (analog) {
		
		
		if (key & PSP2_CTRL_UP)
		{
            looped = false;

			while (1)
            {
				++num;
                if (num > wp_supershotgun)
                {
                    if (looped)
                    {
                        num = plyrweap->readyweapon;
                        break;
                    }

                    num = wp_fist;
                    looped = true;
                }

				if (plyrweap->weaponowned[num])
				{
					plyrweap->pendingweapon = num;
					break;
				}

				
			}

		kbevent.type = ev_keydown;
		kbevent.data1 = KEY_1 + num;
		D_PostEvent(&kbevent);
		}
		else  
		{

		kbevent.type = ev_keyup;
		kbevent.data1 = KEY_1 + num;
		D_PostEvent(&kbevent);
		}  
		
		if (key & PSP2_CTRL_DOWN)
        {
            looped = false;

			while (1)
            {
				--num;
                if (num == -1)
                {
                    if (looped)
                    {
                        num = plyrweap->readyweapon;
                        break;
                    }

                    num = wp_supershotgun;
                    looped = true;
                }
				if (plyrweap->weaponowned[num])
				{
					plyrweap->pendingweapon = num;
					break;
				}
			}

			kbevent.type = ev_keydown;
			kbevent.data1 = KEY_1 + num;
			D_PostEvent(&kbevent);
		}
		}

		if (!analog) {
		if (ctl.ly <= 0x10)
		{
            looped = false;

			while (1)
            {
				++num;
				if (num > wp_supershotgun)
                {
                    if (looped)
                    {
                        num = plyrweap->readyweapon;
                        break;
                    }

                    num = wp_fist;
                    looped = true;
                }

				if (plyrweap->weaponowned[num])
				{
					plyrweap->pendingweapon = num;
					break;
				}
			}

			kbevent.type = ev_keydown;
			kbevent.data1 = KEY_1 + num;
			D_PostEvent(&kbevent);
		}
		else  
        {
			kbevent.type = ev_keyup;
			kbevent.data1 = KEY_1 + num;
			D_PostEvent(&kbevent);
		}
		if (ctl.ly >= 0xD0)
		{
            looped = false;

			while (1)
            {
                --num;
                if (num == -1)
                {
                    if (looped)
                    {
                        num = plyrweap->readyweapon;
                        break;
                    }

                    num = wp_supershotgun;
                    looped = true;
                }

				if (plyrweap->weaponowned[num])
				{
					plyrweap->pendingweapon = num;
					break;
				}
			}

			kbevent.type = ev_keydown;
			kbevent.data1 = KEY_1 + num;
			D_PostEvent(&kbevent);
		}
        }
    }

	if (d_controls[5] == KEY_ENTER || d_controls[5] == KEY_ESCAPE ||
	    d_controls[5] == ' ' || d_controls[5] == KEY_TAB) {
	    if (key & PSP2_CTRL_TRIANGLE)
	    {
	    	kbevent.type = ev_keydown;
	    	kbevent.data1 = d_controls[5];
	    	D_PostEvent(&kbevent);
	    }
	    else  
	    {
	    kbevent.type = ev_keyup;
	    kbevent.data1 = d_controls[5];
	    D_PostEvent(&kbevent);
	    }
	}
	else {
	    if (ctl.buttons & PSP2_CTRL_TRIANGLE)
	    {
	    kbevent.type = ev_keydown;
	    kbevent.data1 = d_controls[5];
	    D_PostEvent(&kbevent);
	    }
	    else  
	    {
	    kbevent.type = ev_keyup;
	    kbevent.data1 = d_controls[5];
	    D_PostEvent(&kbevent);
	    }
	}
	if (d_controls[8] == KEY_ENTER || d_controls[8] == KEY_ESCAPE ||
	    d_controls[8] == ' ' || d_controls[8] == KEY_TAB) {
        if (key & PSP2_CTRL_LTRIGGER)
	    {
	    kbevent.type = ev_keydown;
	    kbevent.data1 = d_controls[8];
	    D_PostEvent(&kbevent);
	    }
	    else  
	    {
	    kbevent.type = ev_keyup;
	    kbevent.data1 = d_controls[8];
	    D_PostEvent(&kbevent);
	    }
	}
	else {
        if (ctl.buttons & PSP2_CTRL_LTRIGGER)
	    {
	    kbevent.type = ev_keydown;
	    kbevent.data1 = d_controls[8];
	    D_PostEvent(&kbevent);
	    }
	    else  
	    {
	    kbevent.type = ev_keyup;
	    kbevent.data1 = d_controls[8];
	    D_PostEvent(&kbevent);
	    }
	}
	if (d_controls[9] == KEY_ENTER || d_controls[9] == KEY_ESCAPE ||
	    d_controls[9] == ' ' || d_controls[9] == KEY_TAB) {
        if (key & PSP2_CTRL_RTRIGGER)
	    {
	    kbevent.type = ev_keydown;
	    kbevent.data1 = d_controls[9];
	    D_PostEvent(&kbevent);
	    }
	    else  
	    {
	    kbevent.type = ev_keyup;
	    kbevent.data1 = d_controls[9];
	    D_PostEvent(&kbevent);
	    }
	}
	else {
        if (ctl.buttons & PSP2_CTRL_RTRIGGER)
	    {
	    kbevent.type = ev_keydown;
	    kbevent.data1 = d_controls[9];
	    D_PostEvent(&kbevent);
	    }
	    else  
	    {
	    kbevent.type = ev_keyup;
	    kbevent.data1 = d_controls[9];
	    D_PostEvent(&kbevent);
	    }
	}
	if (d_controls[6] == KEY_ENTER || d_controls[6] == KEY_ESCAPE ||
	    d_controls[6] == ' ' || d_controls[6] == KEY_TAB) {
	    if(key & PSP2_CTRL_SQUARE){
	    kbevent.type = ev_keydown;
	    kbevent.data1 = d_controls[6];
	    D_PostEvent(&kbevent);
	    }
	    else  
	    {
	    kbevent.type = ev_keyup;
	    kbevent.data1 = d_controls[6];
	    D_PostEvent(&kbevent);
	    }
	}
	else {
	    if(ctl.buttons & PSP2_CTRL_SQUARE){
	    kbevent.type = ev_keydown;
	    kbevent.data1 = d_controls[6];
	    D_PostEvent(&kbevent);
	    }
	    else  
	    {
	    kbevent.type = ev_keyup;
	    kbevent.data1 = d_controls[6];
	    D_PostEvent(&kbevent);
	    }
	}
	if (d_controls[7] == KEY_ENTER || d_controls[7] == KEY_ESCAPE ||
	    d_controls[7] == ' ' || d_controls[7] == KEY_TAB) {
	    if(key & PSP2_CTRL_CIRCLE) {
	        kbevent.type = ev_keydown;
		kbevent.data1 = d_controls[7];
		D_PostEvent(&kbevent);
	    }
	    else  
	    {
	    kbevent.type = ev_keyup;
	    kbevent.data1 = d_controls[7];
	    D_PostEvent(&kbevent);
	    }
	}
	else {
	    if(ctl.buttons & PSP2_CTRL_CIRCLE) {
	        kbevent.type = ev_keydown;
		kbevent.data1 = d_controls[7];
		D_PostEvent(&kbevent);
	    }
	    else  
	    {
	    kbevent.type = ev_keyup;
	    kbevent.data1 = d_controls[7];
	    D_PostEvent(&kbevent);
	    }
	}
	}
}


void NewPrompter() {
	char* buttons[MAX_CONTROL];
	char tempstring[MAX_CONTROL][MAX_TEMP_STRING];
	char* tempkeystring[MAX_CONTROL+2];
	tempkeystring[0] = "WEAPON";
	tempkeystring[1] = "ENTER";
	tempkeystring[2] = "ESC";
	tempkeystring[3] = "USE";
	tempkeystring[4] = "MOVE";
	tempkeystring[5] = "MAP";
	tempkeystring[6] = "RUN";
	tempkeystring[7] = "FIRE";
	tempkeystring[8] = "M_LEFT";
	tempkeystring[9] = "M_RIGHT";
    //tempkeystring[10] = "222";
    //tempkeystring[11] = "266";
    //tempkeystring[12] = "333";
	buttons[0] = "ANALOG   ";
	buttons[1] = "START    ";
	buttons[2] = "SELECT   ";
	buttons[3] = "CROSS    ";
	buttons[4] = "DPAD     ";
	buttons[5] = "TRIANGLE ";
	buttons[6] = "SQUARE   ";
	buttons[7] = "CIRCLE   ";
	buttons[8] = "L_TRIG   ";
	buttons[9] = "R_TRIG   ";
	//buttons[10] = "CPU_CLOCK";
	
	control_bef_ctl = 0;
	pgScreenFrame(2,0);
	pgFillvram(0);
	pgScreenFlipV();
	while (1) {
	    pgFillvram(0);
	    int i = 0;
	    int die = 0;
	    int col;
	    for (i=0; i<MAX_CONTROL; i++) {
	    
		if (d_controls[i] == CKEY_WEAPON) die = 0;
		else if (d_controls[i] == KEY_ENTER) die = 1;
		else if (d_controls[i] == KEY_ESCAPE) die = 2;
		else if (d_controls[i] == ' ') die = 3;
		else if (d_controls[i] == CKEY_MOVE) die = 4;
		else if (d_controls[i] == KEY_TAB) die = 5;
		else if (d_controls[i] == KEY_RSHIFT) die = 6;
		else if (d_controls[i] == KEY_RCTRL) die = 7;
		else if (d_controls[i] == ',') die = 8;
		else if (d_controls[i] == '.') die = 9;
		else if (d_controls[i] == 222) die = 10;
		else if (d_controls[i] == 266) die = 11;
		else if (d_controls[i] == 333) die = 12;
	
		col = rgb2col(255,255,255);
	
		if (i==d_clist_curpos) {
		    col = rgb2col(255,0,0);
		}
	
		sprintf(tempstring[i], "%s = %s",buttons[i],tempkeystring[die]);
        mh_print(5, ((i + 2) * 20), tempstring[i], col, 0, 0);
	    }
	    if (d_clist_curpos != 10) {
	        mh_print(5,255,"X: Exit | O: Change | Triangle: Save To File",rgb2col(50,255,50),0,0);
	    }
	    else {
	        mh_print(5,255,"X: Exit | O: Increment | Square: Decrement | Triangle: Save To File",rgb2col(50,255,50),0,0);
	    }
	    pgScreenFlipV();
	
	    if (control_bef_ctl == 0) {
	        Read_Key2();
	        pgWaitV();
	    }
	    else {
	        unsigned long ankey;
	        while (1) {
	    	    ankey = Read_Key3();
	    	if (ankey != 0) break;
	    	    pgWaitV();
	    	}
	    	new_pad = old_pad = now_pad = ankey;
	    	control_bef_ctl = 0;
	    }
	    
	   
	    if (new_pad & PSP2_CTRL_UP) {
		if (d_clist_curpos > 0) {
			d_clist_curpos--;
			if (d_clist_curpos < d_clist_start) { d_clist_start = d_clist_curpos; }
		}
	    }
	    else if (new_pad & PSP2_CTRL_DOWN) {
		if (d_clist_curpos < (MAX_CONTROL-1)) {
			d_clist_curpos++;
			if (d_clist_curpos >= (d_clist_start+MAX_CONTROL-1)) { d_clist_start++; }
		}
	    }
	
	    else if (new_pad & PSP2_CTRL_CIRCLE) {
	    if (d_clist_curpos != 10) {
	        ButtonChange(tempstring, buttons, tempkeystring);
	    }
	    else {
	        change_cpu_clock(1);  // increment
	    }
	    }
	
	    else if (new_pad & PSP2_CTRL_TRIANGLE) {
	    SaveToFile(tempstring, buttons, tempkeystring);
	    }
	    
	    else if (new_pad & PSP2_CTRL_SQUARE) {
	    if (d_clist_curpos == 10)
	        change_cpu_clock(0); // decrement
	    }
	    
	    else if (new_pad & PSP2_CTRL_CROSS) {
	    break;
	    }
	}
	
}

void change_cpu_clock(boolean change) {

	if (change && d_controls[10] == 222) {
	    d_controls[10] = 266;
	    //scePowerSetClockFrequency(266,266,133);
	}
	
	else if (!change && d_controls[10] == 222) {
	    d_controls[10] = 333;
	    //scePowerSetClockFrequency(333,333,166);
	}
	
	else if (change && d_controls[10] == 266) {
	    d_controls[10] = 333;
	    //scePowerSetClockFrequency(333,333,166);
	}
	
	else if (!change && d_controls[10] == 266) {
	    d_controls[10] = 222;
	    //scePowerSetClockFrequency(222,222,111);
	}
	
	else if (change && d_controls[10] == 333) {
	    d_controls[10] = 222;
	    //scePowerSetClockFrequency(222,222,111);
	}
	
	else {
	    d_controls[10] = 266;
	    //scePowerSetClockFrequency(266,266,133);
	}

}

void ButtonChange(char tempstring[][MAX_TEMP_STRING], const char* buttons[], const char* tempkeystring[]) {
	
	pgFillvram(0);

	int i = 0;
	int die = 0;
	int col;
	int tempact = 0;
	char tempselect[27];
	
	for (i=0; i<MAX_CONTROL; i++) {

	    if (d_controls[i] == CKEY_WEAPON) die = 0;
	    else if (d_controls[i] == KEY_ENTER) die = 1;
	    else if (d_controls[i] == KEY_ESCAPE) die = 2;
	    else if (d_controls[i] == ' ') die = 3;
	    else if (d_controls[i] == CKEY_MOVE) die = 4;
	    else if (d_controls[i] == KEY_TAB) die = 5;
	    else if (d_controls[i] == KEY_RSHIFT) die = 6;
	    else if (d_controls[i] == KEY_RCTRL) die = 7;
	    else if (d_controls[i] == ',') die = 8;
	    else if (d_controls[i] == '.') die = 9;
	    else if (d_controls[i] == 222) die = 10;
	    else if (d_controls[i] == 266) die = 11;
	    else if (d_controls[i] == 333) die = 12;
	
	    col = rgb2col(255,255,255);
	
	    if (i==d_clist_curpos) {
	        col = rgb2col(255,0,0);
	    }
	
        sprintf(tempstring[i], "%s = %s", buttons[i], tempkeystring[die]);
        mh_print(5, ((i + 1) * 20), tempstring[i], col, 0, 0);
	}
	
	if (d_controls[d_clist_curpos] == CKEY_WEAPON) die = 0;
	else if (d_controls[d_clist_curpos] == KEY_ENTER) die = 1;
	else if (d_controls[d_clist_curpos] == KEY_ESCAPE) die = 2;
	else if (d_controls[d_clist_curpos] == ' ') die = 3;
	else if (d_controls[d_clist_curpos] == CKEY_MOVE) die = 4;
	else if (d_controls[d_clist_curpos] == KEY_TAB) die = 5;
	else if (d_controls[d_clist_curpos] == KEY_RSHIFT) die = 6;
	else if (d_controls[d_clist_curpos] == KEY_RCTRL) die = 7;
	else if (d_controls[d_clist_curpos] == ',') die = 8;
	else if (d_controls[d_clist_curpos] == '.') die = 9;
	else if (d_controls[d_clist_curpos] == 222) die = 10;
	else if (d_controls[d_clist_curpos] == 266) die = 11;
	else if (d_controls[d_clist_curpos] == 333) die = 12;
	
	sprintf(tempselect, "Make selection for %s", tempkeystring[die]);
    mh_print(5, 255, tempselect, rgb2col(50, 255, 50), 0, 0);
		
	pgScreenFlipV();

	unsigned long key;
	control_bef_ctl = now_pad;
	
	while (1) {
	    key = Read_Key3();
	    if (key != 0) break;
	    pgWaitV();
	}
	
	if (d_controls[die] == CKEY_WEAPON || d_controls[die] == CKEY_MOVE) {
	    tempact = d_controls[4];
	    d_controls[4] = d_controls[0];
	    d_controls[0] = tempact;
	    
	    if (d_controls[0] == CKEY_MOVE) {
	        analog = 1;
	    }
	    else {
	        analog = 0;
	    }
	}
	else if (key & PSP2_CTRL_START) {
	    tempact = d_controls[1];
	    d_controls[1] = d_controls[d_clist_curpos];
	    d_controls[d_clist_curpos] = tempact;
	}
	else if (key & PSP2_CTRL_SELECT) {
	    tempact = d_controls[2];
	    d_controls[2] = d_controls[d_clist_curpos];
	    d_controls[d_clist_curpos] = tempact;
	}
	else if (key & PSP2_CTRL_CROSS) {
	    tempact = d_controls[3];
	    d_controls[3] = d_controls[d_clist_curpos];
	    d_controls[d_clist_curpos] = tempact;
	}
	else if (key & PSP2_CTRL_TRIANGLE) {
	    tempact = d_controls[5];
	    d_controls[5] = d_controls[d_clist_curpos];
	    d_controls[d_clist_curpos] = tempact;
	}
	else if (key & PSP2_CTRL_SQUARE) {
	    tempact = d_controls[6];
	    d_controls[6] = d_controls[d_clist_curpos];
	    d_controls[d_clist_curpos] = tempact;
	}
	else if (key & PSP2_CTRL_CIRCLE) {
	    tempact = d_controls[7];
	    d_controls[7] = d_controls[d_clist_curpos];
	    d_controls[d_clist_curpos] = tempact;
	}
    else if (key & PSP2_CTRL_LTRIGGER) {
	    tempact = d_controls[8];
	    d_controls[8] = d_controls[d_clist_curpos];
	    d_controls[d_clist_curpos] = tempact;
	}
    else if (key & PSP2_CTRL_RTRIGGER) {
	    tempact = d_controls[9];
	    d_controls[9] = d_controls[d_clist_curpos];
	    d_controls[d_clist_curpos] = tempact;
	}
	
	control_bef_ctl = key;
	
}

void SaveToFile(char tempstring[][MAX_TEMP_STRING], const char* buttons[], const char* tempkeystring[]) {

	int file = 0;
	int bufsz = 0;
	char buff[MAX_TEMP_STRING+1];
	extern char *doomwaddir;
	char config_file[256];
	int i = 0;
	int die = 0;
	for (i=0; i<MAX_CONTROL; i++) {

	    if (d_controls[i] == CKEY_WEAPON) die = 0;
	    else if (d_controls[i] == KEY_ENTER) die = 1;
	    else if (d_controls[i] == KEY_ESCAPE) die = 2;
	    else if (d_controls[i] == ' ') die = 3;
	    else if (d_controls[i] == CKEY_MOVE) die = 4;
	    else if (d_controls[i] == KEY_TAB) die = 5;
	    else if (d_controls[i] == KEY_RSHIFT) die = 6;
	    else if (d_controls[i] == KEY_RCTRL) die = 7;
	    else if (d_controls[i] == ',') die = 8;
	    else if (d_controls[i] == '.') die = 9;
	    else if (d_controls[i] == 222) die = 10;
	    else if (d_controls[i] == 266) die = 11;
	    else if (d_controls[i] == 333) die = 12;

	    sprintf(tempstring[i], "%s = %s", buttons[i],tempkeystring[die]);
	}
	sprintf(config_file, "%sconfig.ini", doomwaddir);
	file = sceIoOpen(config_file, PSP2_O_CREAT|PSP2_O_WRONLY|PSP2_O_TRUNC, 0777);

	for (i=0;i<MAX_CONTROL;i++) {
	    
	    bufsz = sprintf( buff, "%s\n",tempstring[i]);
	    sceIoWrite(file, buff, bufsz);
	
	}
	
	if (file) {
	    sceIoClose(file);
	}

}

//
// I_StartTic
//
void I_StartTic (void)
{
	I_GetEvent( ); 
}


//
// I_UpdateNoBlit
//
void I_UpdateNoBlit (void)
{
    // what is this?
}

//
// I_FinishUpdate
//

#define FPS_TIMES_COUNT 60
uint64_t lastTick = 0;
int fpsTimes[FPS_TIMES_COUNT];
int fpsIndex = 0;
unsigned char fpstext[16];

void DisplayFPS()
{
    uint64_t currentTick = 0;

    int result = sceRtcGetCurrentTick(&currentTick);
    if (result != 0)
    {
        printf("sceRtcGetCurrentTick : 0x%x\n", currentTick);
    }

    uint64_t difference = currentTick - lastTick;
    lastTick = currentTick;

    int fps = sceRtcGetTickResolution() / (int) difference;
    fpsTimes[fpsIndex] = fps;

    if (++fpsIndex == FPS_TIMES_COUNT)
    {
        fpsIndex = 0;
    }

    fps = 0;

    int i;
    for (i = 0; i < FPS_TIMES_COUNT; ++i)
    {
        fps += fpsTimes[i];
    }

    fps /= FPS_TIMES_COUNT;

    PSP2_Video_FillScreen(SCREEN_W * 200);

    sprintf(fpstext, "FPS: %d", fps);
    mh_print(5, 5, fpstext, RGBA8(0, 255, 0, 255), 0, 0);

}

void I_FinishUpdate (void)
{
    DisplayFPS();

    PSP2_Video_DrawBuffer();

 //
	//sceKernelDcacheWritebackAll();
	//sceGuStart(0,list);
	//sceGuClearColor(0xff000000);
	//sceGuClearDepth(0);
	//sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);


	//sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);
 //
	//sceGuClutMode(GU_PSM_8888,0,0xff,0); // 32-bit palette
	//sceGuClutLoad((32),colors); // upload 32*8 entries (256)

	//sceGuTexMode(GU_PSM_T8,0,0,0);  
	//sceGuTexImage(0,512,256,320, ilineptr);
	//sceGuTexFunc(GU_TFX_REPLACE,0);
	//sceGuTexFilter(GU_LINEAR,GU_LINEAR);
	//sceGuTexScale(1,1);
	//sceGuTexOffset(0,0);
	//sceGuAmbientColor(0xffffffff);

	//// render sprite

	//sceGuColor(0xffffffff);
	//struct Vertex* vertices = (struct Vertex*)sceGuGetMemory(2 * sizeof(struct Vertex));
	//vertices[0].u = 0; vertices[0].v = 0;
	//vertices[0].x = 0; vertices[0].y = 0; vertices[0].z = 0;
	//vertices[1].u = 320; vertices[1].v = 200;
	//vertices[1].x = 480; vertices[1].y = 272; vertices[1].z = 0;
	//sceGuDrawArray(GU_SPRITES,GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_2D,2,0,vertices);

	//// wait for next frame

	//sceGuFinish();
	//sceGuSync(0,0);

 //	
	//sceGuSwapBuffers();
 
}


//
// I_ReadScreen
//
void I_ReadScreen (byte* scr)
{
    memcpy (scr, screens[0], SCREENWIDTH*SCREENHEIGHT);
}


//
// I_SetPalette
//
void I_SetPalette (byte* palette)
{
    int i;

    uint32_t* colors256 = PSP2_GetPalettePtr(); //colors;
	
	for (i = 0; i < 256; ++i)
	{
		unsigned int r = (gammatable[usegamma][*palette++])&0xff;
		unsigned int g = (gammatable[usegamma][*palette++])&0xff;
		unsigned int b = (gammatable[usegamma][*palette++])&0xff;

		//*(colors256++) = (0 & 0xFF << 24)|
		//	             (b << 16)|
		//			     (g << 8) |
		//				 (r);

        *(colors256++) = RGBA8(r, g, b, 0xFF);
	}
}


void I_InitGraphics(void)
{
	static int	firsttime=1;
    long video_w, video_h, w, h;
    long video_bpp;
    
    if (!firsttime)
	return;
    firsttime = 0;
 

	multiply = 1;

    if (M_CheckParm("-2"))
	multiply = 2;

    if (M_CheckParm("-3"))
	multiply = 3;

    // check if the user wants to grab the mouse (quite unnice)
    grabMouse = !!M_CheckParm("-grabmouse");

    video_w = w = 320;
    video_h = h = 200;
    video_bpp = 8;

    PSP2_Video_Init();

    screens[0] = PSP2_Video_GetVideoPtr();  //(unsigned char *) malloc(SCREENWIDTH * SCREENHEIGHT);

	//sceKernelDcacheWritebackAll();

	//// setup GU
	//
	//sceGuInit();
	//sceGuStart(0,list);

	//sceGuDrawBuffer(GU_PSM_8888,(void*)0,BUF_WIDTH);
	//sceGuDispBuffer(SCR_WIDTH,SCR_HEIGHT,(void*)FRAME_SIZE,BUF_WIDTH);
	//sceGuDepthBuffer((void*)(FRAME_SIZE*2),BUF_WIDTH);
	//sceGuOffset(2048 - (SCR_WIDTH/2),2048 - (SCR_HEIGHT/2));
	//sceGuViewport(2048,2048,SCR_WIDTH,SCR_HEIGHT);
	//sceGuDepthRange(0xc350,0x2710);
	//sceGuScissor(0,0,SCR_WIDTH,SCR_HEIGHT);
	//sceGuEnable(GU_SCISSOR_TEST);
	//sceGuFrontFace(GU_CW);
	//sceGuEnable(GU_TEXTURE_2D);
	//sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);
	//sceGuFinish();
	//sceGuSync(0,0);

	//sceDisplayWaitVblankStart();
	//pspDebugScreenClear();

	//sceGuDisplay(GU_TRUE);
	
}

void I_FinishUpdate2 () {

    PSP2_Video_FillScreen(0);

	//sceGuDrawBuffer(GU_PSM_8888,(void*)0,BUF_WIDTH);
	//sceGuDispBuffer(SCR_WIDTH,SCR_HEIGHT,(void*)FRAME_SIZE,BUF_WIDTH);
	//sceGuDepthBuffer((void*)(FRAME_SIZE*2),BUF_WIDTH);
	//sceGuOffset(2048 - (SCR_WIDTH/2),2048 - (SCR_HEIGHT/2));
	//sceGuViewport(2048,2048,SCR_WIDTH,SCR_HEIGHT);
	//sceGuDepthRange(0xc350,0x2710);
	//sceGuScissor(0,0,SCR_WIDTH,SCR_HEIGHT);
	//sceGuEnable(GU_SCISSOR_TEST);
	//sceGuFrontFace(GU_CW);
	//sceGuEnable(GU_TEXTURE_2D);
	//sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);
	//sceGuFinish();
	//sceGuSync(0,0);

	//sceDisplayWaitVblankStart();
	//pspDebugScreenClear();

	//sceGuDisplay(GU_TRUE);
}
