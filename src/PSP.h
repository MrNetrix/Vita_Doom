#ifndef __PSP_H__
#define __PSP_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define    LINESIZE        512
#define    PIXELSIZE       1
#define FRAMESIZE                          0x44000
#define SCREEN_WIDTH  480
#define SCREEN_HEIGHT 272

#include <psp2/types.h>
#include <psp2/ctrl.h>
#include <psp2/power.h>
#include <psp2/touch.h>
#include <psp2/io/fcntl.h>
#include <psp2/moduleinfo.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/sysmem.h>

#include "doomtype.h"

#define align_mem(addr, align) (((addr) + ((align) - 1)) & ~((align) - 1))
#define lerp(value, from_max, to_max) ((((value*10) * (to_max*10))/(from_max*10))/10)

#define RGBA8(r, g, b, a)      ((((a)&0xFF)<<24) | (((b)&0xFF)<<16) | (((g)&0xFF)<<8) | (((r)&0xFF)<<0))

#define SCREEN_W 960
#define SCREEN_H 544

#define RED   RGBA8(255,0,0,255)
#define GREEN RGBA8(0,255,0,255)
#define BLUE  RGBA8(0,0,255,255)
#define BLACK RGBA8(0,0,0,255)
#define WHITE RGBA8(255,255,255,255)

//#define sceCtrlPeekBufferPositive sceCtrlReadBufferPositive

void PSP2_Video_Init();
void PSP2_Video_FillScreen();
void PSP2_Video_DrawBuffer();

void font_draw_char(int x, int y, uint32_t color, char c);
void font_draw_string(int x, int y, uint32_t color, const char *string);
void font_draw_stringf(int x, int y, uint32_t color, const char *s, ...);

struct timeval {
    long    tv_sec;         /* seconds */
    long    tv_usec;        /* and microseconds */
};

struct timezone {
    int tz_minuteswest;     /* minutes west of Greenwich */
    int tz_dsttime;         /* type of DST correction */
};

unsigned long new_pad;
unsigned long old_pad;
unsigned long now_pad;

enum { 
    TYPE_DIR=0x10, 
    TYPE_FILE=0x20 
};
 
typedef struct dirent { 
    unsigned long unk0; 
    unsigned long type; 
    unsigned long size; 
    unsigned long unk[19]; 
    char name[0x108]; 
	unsigned char dmy[128];
} dirent_t;

void pgWaitV();
uint32_t rgb2col(unsigned char r, unsigned char g, unsigned char b);
void Get_DirList(char *path);
char *pgGetVramAddr(unsigned long x,unsigned long y);
void Draw_All(void);
void pgFillvram(unsigned long color);
int Control(void);
void Read_Key2();
void pgScreenFlip();
void pgScreenFlipV();
unsigned short num2elisa(unsigned short c);
void Draw_Char_Hankaku(int x,int y,unsigned char ch,int col,int backcol,int fill);
void Draw_Char_Zenkaku(int x,int y,unsigned char u,unsigned char d,int col,int backcol,int fill);
void mh_print(int x, int y, unsigned char *str, uint32_t col, int backcol, int fill);
void pgInit();
void pgScreenFrame(long mode,long frame);

#define PSP2_DIR(path) "cache0:/VitaDefilerClient/" path

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PSP_H__ */
