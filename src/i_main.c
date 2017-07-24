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
//	Main program, simply calls D_DoomMain high level loop.
//
//-----------------------------------------------------------------------------

static const char
rcsid[] = "$Id: i_main.c,v 1.4 1997/02/03 22:45:10 b1 Exp $";

#include <stdio.h>
#include <stdlib.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/audioout.h>
#include "doomdef.h"

#include "m_argv.h"
#include "d_main.h"
#include "PSP.h"
#include "fontb.h"

long pg_screenmode;

extern char *doomwaddir;
char doomwaddir2[256];
 
extern void sound_callback(void *buf, unsigned int reqn);

SceCtrlData ctl;

// ---------------------------------------------------------------------------------
#define MAXDEPTH  16
#define MAXDIRNUM 1024
#define MAXPATH   0x108
#define PATHLIST_H 20
#define REPEAT_TIME 0x40000
#define BUFSIZE		65536
#define DEFAULT_SCREEN_SCALE	SCREEN_SCALE_FULL

dirent_t		dlist[MAXDIRNUM];
int				dlist_num;
char			now_path[MAXPATH];
extern char			target[MAXPATH];
char			path_tmp[MAXPATH];
int				dlist_start;
int				dlist_curpos;
int				cbuf_start[MAXDEPTH];
int				cbuf_curpos[MAXDEPTH];
int				now_depth;
char			buf[BUFSIZE];
int				screen_res;

int debug_res = 0x0;

int main(int argc, char** argv)
{
#ifdef USE_DEBUGNET
    int ret = debugNetInit("255.255.255.255", 18194, DEBUG);
    printf("debugNetInit: %d\n", ret);
#endif

    printf("DOOM started\n");

    setbuf(stdout, NULL);

    int res = scePowerSetArmClockFrequency(50);
    if (res != 0x0)
    {
        printf("scePowerSetArmClockFrequency failed! (0x%08x) (Normal if not using PSM Unity 1.06. No clock speed change for you.)\n", res);
    }

    myargc = 0;
    myargv = 0;

    doomwaddir = PSP2_DIR("Doom/");
    strcpy_s(doomwaddir2, 256, doomwaddir);
    strcat(doomwaddir2, "WADS/");

    printf("WAD folder: %s\n", doomwaddir2);

	Vita_Audio_Init();

	//sceCtrlSetSamplingCycle(0);

	//int result = sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
 //   if (result < 0)
 //   {
 //       printf("sceCtrlSetSamplingMode : 0x%x", result);
 //   }

	//SetupCallbacks();
        screen_res = DEFAULT_SCREEN_SCALE;
        pgInit(screen_res);
        pgScreenFrame(2,0);
        pgFillvram(0);

    Get_DirList(doomwaddir2);
    dlist_start  = 0;
    dlist_curpos = 0;
    now_depth    = 0;

while(1) {
    Draw_All();
switch(Control()) {
		case 1:
			Get_DirList(doomwaddir2);
			break;
		case 2:
                        pgFillvram(0);
                        strcpy_s(target, 264, doomwaddir2);
                        strcat(target, dlist[dlist_curpos].name);

                        pgScreenFlipV();
                        //pspDebugScreenInit();

    D_DoomMain (); 
        }
    }    

    return 0;

}

void pgInit(int scale)
{
    PSP2_Video_Init(scale);

	//sceDisplaySetMode(0,SCREEN_WIDTH,SCREEN_HEIGHT);
	pgScreenFrame(0,0);
}

void pgScreenFrame(long mode,long frame)
{
	pg_screenmode=mode;
	frame=(frame?1:0);

	if (mode==0) {
		//screen off
        PSP2_Video_DrawBuffer();
	} else if (mode==1) {
        //show/draw same
        PSP2_Video_DrawBuffer();
	} else if (mode==2) {
        //show/draw different
        PSP2_Video_DrawBuffer();
	}
}

void pgScreenFlipV()
{
	pgScreenFlip();
}

void pgScreenFlip()
{
    PSP2_Video_DrawBuffer();
}

unsigned short num2elisa(unsigned short c) {
	if (c > 4374) {
		return 0x6b;
	} else if (c >= 1410) {
		return c + (0x20c - 1410);
	} else if (c >= 690) {
		return 0x6b;
	} else if (c >= 658) {
		return c + (0x1ec - 658);
	} else if (c >= 612) {
		return c + (0x1cb - 612);
	} else if (c >= 564) {
		return c + (0x1aa - 564);
	} else if (c >= 502) {
		return c + (0x192 - 502);
	} else if (c >= 470) {
		return c + (0x17a - 470);
	} else if (c >= 376) {
		return c + (0x124 - 376);
	} else if (c >= 282) {
		return c + (0xd1 - 282);
	} else if (c >= 252) {
		return c + (0xb7 - 252);
	} else if (c >= 220) {
		return c + (0x9d - 220);
	} else if (c >= 203) {
		return c + (0x93 - 203);
	} else if (c >= 187) {
		return 0x92;
	} else if (c >= 175) {
		return c + (0x8a - 203);
	} else if (c >= 153) {
		return c + (0x7b - 153);
	} else if (c >= 135) {
		return c + (0x74 - 135);
	} else if (c >= 119) {
		return c + (0x6c - 119);
	} else {
		return c;
	}
}

void Draw_Char_Hankaku(int x,int y,unsigned char ch,int col,int backcol,int fill) {
	//unsigned short *vr;
	//unsigned char  *fnt;
	//unsigned char  pt;
	//int x1,y1;

	//// mapping
	//if (ch<0x20) {
	//	ch = 0;
	//} else if (ch<0x80) {
	//	ch -= 0x20;
	//} else if (ch<0xa0) {
	//	ch = 0;
	//} else {
	//	ch -= 0x40;
	//}
	//fnt = (unsigned char *)&hankaku_font10[ch*10];

	//// draw
	//vr = (unsigned short *)pgGetVramAddr(x,y);
	//for(y1=0;y1<10;y1++) {
	//	pt = *fnt++;
	//	for(x1=0;x1<5;x1++) {
	//		if (pt & 1) {
	//			*vr = col;
	//		} else {
	//			if (fill) *vr = backcol;
	//		}
	//		vr++;
	//		pt = pt >> 1;
	//	}
	//	vr += LINESIZE-5;
	//}
}

void Draw_Char_Zenkaku(int x,int y,unsigned char u,unsigned char d,int col,int backcol,int fill) {
	//unsigned short *vr;
	//unsigned short *fnt;
	//unsigned short pt;
	//int x1,y1;

	//// mapping
	//if (d > 0x7F) d--;
	//d -= 0x40; u -= 0x81;
	//fnt = (unsigned short *)&zenkaku_font10[num2elisa(u*0xbc+d)*10];

	//// draw
	//vr = (unsigned short *)pgGetVramAddr(x,y);
	//for(y1=0;y1<10;y1++) {
	//	pt = *fnt++;
	//	for(x1=0;x1<10;x1++) {
	//		if (pt & 1) {
	//			*vr = col;
	//		} else {
	//			if (fill) *vr = backcol;
	//		}
	//		vr++;
	//		pt = pt >> 1;
	//	}
	//	vr += LINESIZE-10;
	//}
}

void mh_print(int x,int y,unsigned char *str, uint32_t col,int backcol,int fill) {
	unsigned char ch = 0,bef = 0;

    font_draw_string(x, y, col, str);

    return;

	while(*str != 0) {
		ch = *str++;
		if (bef!=0) {
			Draw_Char_Zenkaku(x,y,bef,ch,col,backcol,fill);

			x+=10;
			bef=0;
		} else {
			if (((ch>=0x80) && (ch<0xa0)) || (ch>=0xe0)) {
				bef = ch;
			} else {
				Draw_Char_Hankaku(x,y,ch,col,backcol,fill);
				x+=5;
			}
		}
	}
}

void Get_DirList(char *path) {
	int ret,fd;

	// Directory read
	fd = sceIoDopen(path);

	dlist_num = 0;
	ret = 1;
	while((ret>0) && (dlist_num<MAXDIRNUM)) {
		ret = sceIoDread(fd, &dlist[dlist_num]);
		if (dlist[dlist_num].name[0] == '.') continue;
		if (ret>0) dlist_num++;
	}
	sceIoDclose(fd);

	if (dlist_start  >= dlist_num) { dlist_start  = dlist_num-1; }
	if (dlist_start  <  0)         { dlist_start  = 0;           }
	if (dlist_curpos >= dlist_num) { dlist_curpos = dlist_num-1; }
	if (dlist_curpos <  0)         { dlist_curpos = 0;           }
}

void Draw_All(void) {
	int			i,col;

	pgFillvram(0);
	
    strncpy(path_tmp, doomwaddir2, 128);

    mh_print(5, 5, "Please choose a game to launch (Press Circle to select)", rgb2col(255, 255, 255), 0, 0);
    mh_print(310, 245, "Vita Doom - Release 1", rgb2col(255, 0, 0), 0, 0);
    mh_print(245, 270, "based on DoomPSP-0.04 S.S. b5", rgb2col(255, 255, 0), 0, 0);

    /*char errtext[64];
    sprintf(errtext, "Error: 0x%x", res);

    mh_print(10, 300, errtext, RGBA8(255, 0, 0, 2555), 0, 0);*/

    //Display Screen Resolution
    switch (screen_res)
    {
        case SCREEN_SCALE_FULL:
            mh_print(0, 500, "Screen Resolution (press select to change): Fullscreen", rgb2col(255, 255, 0), 0, 0);
            break;
        case SCREEN_SCALE_FIT:
            mh_print(0, 500, "Screen Resolution (press select to change): Fit to Screen", rgb2col(255, 255, 0), 0, 0);
            break;
        case SCREEN_SCALE_ORIG:
            mh_print(0, 500, "Screen Resolution (press select to change): Original", rgb2col(255, 255, 0), 0, 0);
            break;
    }

    if (dlist_num == 0)
    {
        mh_print(32, 40, "No WADs found... Put WADs in:", rgb2col(255, 0, 0), 0, 0);
        mh_print(32, 60, path_tmp, rgb2col(255, 0, 0), 0, 0);
    }
    else
    {
        i = dlist_start;
        while (i < (dlist_start + PATHLIST_H)) {
            if (i < dlist_num) {
                col = rgb2col(255, 255, 255);
                if (dlist[i].type & TYPE_DIR) {
                    col = rgb2col(255, 255, 0);
                }
                if (i == dlist_curpos) {
                    col = rgb2col(255, 0, 0);
                }
                strncpy(path_tmp, dlist[i].name, 40);
                mh_print(32, ((i - dlist_start) + 2) * 20, path_tmp, col, 0, 0);
            }
            i++;
        }
    }

    pgScreenFlipV();
}

void pgFillvram(unsigned long color)
{
    PSP2_Video_FillScreen(color);
}

uint32_t rgb2col(unsigned char r, unsigned char g, unsigned char b) {
    return RGBA8(r, g, b, 0xFF);
}

void Read_Key2() {
	static int n=0;
    sceCtrlPeekBufferPositive(0, (SceCtrlData *) &ctl, 1);

	if (ctl.ly >= 0xD0) ctl.buttons|=SCE_CTRL_DOWN;  // DOWN
	if (ctl.ly <= 0x30) ctl.buttons|=SCE_CTRL_UP;    // UP
	if (ctl.lx <= 0x30) ctl.buttons|=SCE_CTRL_LEFT;  // LEFT
	if (ctl.lx >= 0xD0) ctl.buttons|=SCE_CTRL_RIGHT; // RIGHT

	now_pad = ctl.buttons;
	new_pad = now_pad & ~old_pad;
	if(old_pad==now_pad){
		n++;
		if(n>=25){
			new_pad=now_pad;
			n = 20;
		}
	}else{
		n=0;
		old_pad = now_pad;
	}
}

int Control(void) {
	
	int i;

	Read_Key2();
	pgWaitV();
	
	if (new_pad & SCE_CTRL_UP) {
		if (dlist_curpos > 0) {
			dlist_curpos--;
			if (dlist_curpos < dlist_start) { dlist_start = dlist_curpos; }
		}
	}
	if (new_pad & SCE_CTRL_DOWN) {
		if (dlist_curpos < (dlist_num-1)) {
			dlist_curpos++;
			if (dlist_curpos >= (dlist_start+PATHLIST_H)) { dlist_start++; }
		}
	}
	if (new_pad & SCE_CTRL_LEFT) {
		dlist_curpos -= PATHLIST_H;
		if (dlist_curpos <  0)          { dlist_curpos = 0;           }
		if (dlist_curpos < dlist_start) { dlist_start = dlist_curpos; }
	}
	if (new_pad & SCE_CTRL_RIGHT) {
		dlist_curpos += PATHLIST_H;
		if (dlist_curpos >= dlist_num) { dlist_curpos = dlist_num-1; }
		while (dlist_curpos >= (dlist_start+PATHLIST_H)) { dlist_start++; }
	}

	if (new_pad & SCE_CTRL_CIRCLE) {
		if (dlist[dlist_curpos].type & TYPE_DIR) {
			if (now_depth<MAXDEPTH) {
				// pathÚ®
				strcat(doomwaddir2, dlist[dlist_curpos].name);
				strcat(doomwaddir2, "/");
				cbuf_start[now_depth] = dlist_start;
				cbuf_curpos[now_depth] = dlist_curpos;
				dlist_start  = 0;
				dlist_curpos = 0;
				now_depth++;
				return 1;
			}
		} else {
			//
			return 2;
			for(i=0;i<MAXPATH;i++) {
				if (dlist[dlist_curpos].name[i]==0) break;
			}
			if (i>2) {
				if (((dlist[dlist_curpos].name[i-3] & 0xDF) == '.') &&
				    ((dlist[dlist_curpos].name[i-2] & 0xDF) == 'G') &&
				    ((dlist[dlist_curpos].name[i-1] & 0xDF) == 'B')) {
					return 2;
				}
				i--;
			}
		}
	}
	if (new_pad & SCE_CTRL_CROSS) {
		if (now_depth > 0) {
			// pathÚ®
			for(i=0;i<MAXPATH;i++) {
				if (doomwaddir2[i]==0) break;
			}
			i--;
			while(i>4) {
				if (doomwaddir2[i-1]=='/') {
					doomwaddir[i]=0;
					break;
				}
				i--;
			}
			now_depth--;
			dlist_start  = cbuf_start[now_depth];
			dlist_curpos = cbuf_curpos[now_depth];
			return 1;
		}
	}
	if (new_pad & SCE_CTRL_SELECT) {
		Change_Resolution();
	}
	
	return 0;
}

void pgWaitV()
{
    sceDisplayWaitVblankStart();
}

void Change_Resolution()
{
	switch(screen_res)
	{
		case SCREEN_SCALE_FULL:
			screen_res = SCREEN_SCALE_FIT;
			break;
		case SCREEN_SCALE_FIT:
			screen_res = SCREEN_SCALE_ORIG;
			break;
		case SCREEN_SCALE_ORIG:
			screen_res = SCREEN_SCALE_FULL;
			break;
	}
	pgInit(screen_res);
}

void Vita_Audio_Thread() {
	int size = 256;
	int freq = 48000;
	int mode = SCE_AUDIO_OUT_MODE_STEREO;
	int port = sceAudioOutOpenPort(SCE_AUDIO_OUT_PORT_TYPE_BGM, size, freq, mode);
	void* wave_buf[SCE_AUDIO_MAX_LEN]={0};

	while (true) {
		sound_callback(wave_buf, size);
		sceAudioOutOutput(port, wave_buf);
	}
}

void Vita_Audio_Init()
{
	SceUID thid;
	thid = sceKernelCreateThread("audio_thread", &Vita_Audio_Thread, 0x10000100, SCE_AUDIO_MAX_LEN*4, 0, 0, NULL);
	sceKernelStartThread(thid, 0, NULL);
}
