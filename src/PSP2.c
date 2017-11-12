
#include <stdarg.h>

#include <psp2/display.h>
#include <psp2/gxm.h>
#include <psp2/kernel/sysmem.h>

#include "PSP2.h"
#include <vita2d.h>

#include "doomdef.h"

const unsigned char msx_font[] __attribute((aligned(4))) =
"\x00\x00\x00\x00\x00\x00\x00\x00\x20\x20\x20\x20\x00\x00\x20\x00"
"\x50\x50\x50\x00\x00\x00\x00\x00\x50\x50\xf8\x50\xf8\x50\x50\x00"
"\x20\x78\xa0\x70\x28\xf0\x20\x00\xc0\xc8\x10\x20\x40\x98\x18\x00"
"\x40\xa0\x40\xa8\x90\x98\x60\x00\x10\x20\x40\x00\x00\x00\x00\x00"
"\x10\x20\x40\x40\x40\x20\x10\x00\x40\x20\x10\x10\x10\x20\x40\x00"
"\x20\xa8\x70\x20\x70\xa8\x20\x00\x00\x20\x20\xf8\x20\x20\x00\x00"
"\x00\x00\x00\x00\x00\x20\x20\x40\x00\x00\x00\x78\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x60\x60\x00\x00\x00\x08\x10\x20\x40\x80\x00"
"\x70\x88\x98\xa8\xc8\x88\x70\x00\x20\x60\xa0\x20\x20\x20\xf8\x00"
"\x70\x88\x08\x10\x60\x80\xf8\x00\x70\x88\x08\x30\x08\x88\x70\x00"
"\x10\x30\x50\x90\xf8\x10\x10\x00\xf8\x80\xe0\x10\x08\x10\xe0\x00"
"\x30\x40\x80\xf0\x88\x88\x70\x00\xf8\x88\x10\x20\x20\x20\x20\x00"
"\x70\x88\x88\x70\x88\x88\x70\x00\x70\x88\x88\x78\x08\x10\x60\x00"
"\x00\x00\x20\x00\x00\x20\x00\x00\x00\x00\x20\x00\x00\x20\x20\x40"
"\x18\x30\x60\xc0\x60\x30\x18\x00\x00\x00\xf8\x00\xf8\x00\x00\x00"
"\xc0\x60\x30\x18\x30\x60\xc0\x00\x70\x88\x08\x10\x20\x00\x20\x00"
"\x70\x88\x08\x68\xa8\xa8\x70\x00\x20\x50\x88\x88\xf8\x88\x88\x00"
"\xf0\x48\x48\x70\x48\x48\xf0\x00\x30\x48\x80\x80\x80\x48\x30\x00"
"\xe0\x50\x48\x48\x48\x50\xe0\x00\xf8\x80\x80\xf0\x80\x80\xf8\x00"
"\xf8\x80\x80\xf0\x80\x80\x80\x00\x70\x88\x80\xb8\x88\x88\x70\x00"
"\x88\x88\x88\xf8\x88\x88\x88\x00\x70\x20\x20\x20\x20\x20\x70\x00"
"\x38\x10\x10\x10\x90\x90\x60\x00\x88\x90\xa0\xc0\xa0\x90\x88\x00"
"\x80\x80\x80\x80\x80\x80\xf8\x00\x88\xd8\xa8\xa8\x88\x88\x88\x00"
"\x88\xc8\xc8\xa8\x98\x98\x88\x00\x70\x88\x88\x88\x88\x88\x70\x00"
"\xf0\x88\x88\xf0\x80\x80\x80\x00\x70\x88\x88\x88\xa8\x90\x68\x00"
"\xf0\x88\x88\xf0\xa0\x90\x88\x00\x70\x88\x80\x70\x08\x88\x70\x00"
"\xf8\x20\x20\x20\x20\x20\x20\x00\x88\x88\x88\x88\x88\x88\x70\x00"
"\x88\x88\x88\x88\x50\x50\x20\x00\x88\x88\x88\xa8\xa8\xd8\x88\x00"
"\x88\x88\x50\x20\x50\x88\x88\x00\x88\x88\x88\x70\x20\x20\x20\x00"
"\xf8\x08\x10\x20\x40\x80\xf8\x00\x70\x40\x40\x40\x40\x40\x70\x00"
"\x00\x00\x80\x40\x20\x10\x08\x00\x70\x10\x10\x10\x10\x10\x70\x00"
"\x20\x50\x88\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xf8\x00"
"\x40\x20\x10\x00\x00\x00\x00\x00\x00\x00\x70\x08\x78\x88\x78\x00"
"\x80\x80\xb0\xc8\x88\xc8\xb0\x00\x00\x00\x70\x88\x80\x88\x70\x00"
"\x08\x08\x68\x98\x88\x98\x68\x00\x00\x00\x70\x88\xf8\x80\x70\x00"
"\x10\x28\x20\xf8\x20\x20\x20\x00\x00\x00\x68\x98\x98\x68\x08\x70"
"\x80\x80\xf0\x88\x88\x88\x88\x00\x20\x00\x60\x20\x20\x20\x70\x00"
"\x10\x00\x30\x10\x10\x10\x90\x60\x40\x40\x48\x50\x60\x50\x48\x00"
"\x60\x20\x20\x20\x20\x20\x70\x00\x00\x00\xd0\xa8\xa8\xa8\xa8\x00"
"\x00\x00\xb0\xc8\x88\x88\x88\x00\x00\x00\x70\x88\x88\x88\x70\x00"
"\x00\x00\xb0\xc8\xc8\xb0\x80\x80\x00\x00\x68\x98\x98\x68\x08\x08"
"\x00\x00\xb0\xc8\x80\x80\x80\x00\x00\x00\x78\x80\xf0\x08\xf0\x00"
"\x40\x40\xf0\x40\x40\x48\x30\x00\x00\x00\x90\x90\x90\x90\x68\x00"
"\x00\x00\x88\x88\x88\x50\x20\x00\x00\x00\x88\xa8\xa8\xa8\x50\x00"
"\x00\x00\x88\x50\x20\x50\x88\x00\x00\x00\x88\x88\x98\x68\x08\x70"
"\x00\x00\xf8\x10\x20\x40\xf8\x00\x18\x20\x20\x40\x20\x20\x18\x00"
"\x20\x20\x20\x00\x20\x20\x20\x00\xc0\x20\x20\x10\x20\x20\xc0\x00"
"\x40\xa8\x10\x00\x00\x00\x00\x00\x00\x00\x20\x50\xf8\x00\x00\x00"
"\x70\x88\x80\x80\x88\x70\x20\x60\x90\x00\x00\x90\x90\x90\x68\x00";


vita2d_texture *pal_tex = NULL, *font_tex = NULL;
byte *paltex_data;
uint32_t *palette_data, *font_data;

int palette_length = 256;

float screen_x;
float screen_y;
float screen_scale_w;
float screen_scale_h;

void draw_pixel(uint32_t x, uint32_t y, uint32_t color)
{
    font_data[x + y * SCREEN_W] = color;
}

void font_draw_char(int x, int y, uint32_t color, char c)
{
    unsigned char *font = (unsigned char *) (msx_font + (c - (uint32_t)' ') * 8);
    int i, j, pos_x, pos_y;
    for (i = 0; i < 8; ++i) {
        pos_y = y + i * 2;
        for (j = 0; j < 8; ++j) {
            pos_x = x + j * 2;
            if ((*font & (128 >> j))) {
                draw_pixel(pos_x + 0, pos_y + 0, color);
                draw_pixel(pos_x + 1, pos_y + 0, color);
                draw_pixel(pos_x + 0, pos_y + 1, color);
                draw_pixel(pos_x + 1, pos_y + 1, color);
            }
        }
        ++font;
    }
}

void font_draw_string(int x, int y, uint32_t color, const char *string)
{
    if (string == NULL) return;

    int startx = x;
    const char *s = string;

    while (*s) {
        if (*s == '\n') {
            x = startx;
            y += 16;
        }
        else if (*s == ' ') {
            x += 16;
        }
        else if (*s == '\t') {
            x += 16 * 4;
        }
        else {
            font_draw_char(x, y, color, *s);
            x += 16;
        }
        ++s;
    }
}

void font_draw_stringf(int x, int y, uint32_t color, const char *s, ...)
{
    char buf[256];
    va_list argptr;
    va_start(argptr, s);
    vsnprintf(buf, sizeof(buf), s, argptr);
    va_end(argptr);
    font_draw_string(x, y, color, buf);
}

void ScreenFlash(uint32_t color)
{
    color = RGBA8(128, 128, 128, 255);

    vita2d_set_clear_color(color);

    vita2d_start_drawing();

    vita2d_clear_screen();

    vita2d_end_drawing();
    vita2d_swap_buffers();

    sceKernelDelayThread(1000 * 100);
}

void PSP2_Video_Init(int screen_scale, boolean bilinear_enabled)
{
    //ScreenFlash(RGBA8(255, 0, 0, 255));
    //ScreenFlash(RGBA8(0, 255, 0, 255));
    //ScreenFlash(RGBA8(0, 0, 255, 255));
    //ScreenFlash(RGBA8(0, 0, 0, 255));

    //Setup screen scale
    switch (screen_scale)
    {
        case SCREEN_SCALE_NONE:
            break;
        case SCREEN_SCALE_FIT_4_TO_3:
            screen_scale_h = (float)SCREEN_H/(float)SCREENHEIGHT;
            screen_scale_w = screen_scale_h*((float)SCREENHEIGHT/(float)SCREENWIDTH)*(4.0f/3.0f);
            screen_x = ((float)SCREEN_W-screen_scale_w*(float)SCREENWIDTH)/2.0f;
            screen_y = 0;
            break;
        case SCREEN_SCALE_FIT_16_TO_10:
            screen_scale_h = (float)SCREEN_H/(float)SCREENHEIGHT;
            screen_scale_w = screen_scale_h*((float)SCREENHEIGHT/(float)SCREENWIDTH)*(16.0f/10.0f);
            screen_x = ((float)SCREEN_W-screen_scale_w*(float)SCREENWIDTH)/2.0f;
            screen_y = 0;
            break;
        case SCREEN_SCALE_ORIG:
            screen_x = 150;
            screen_y = 75;
            screen_scale_w = 2;
            screen_scale_h = 2;
            break;
        case SCREEN_SCALE_FULL:
            screen_x = 0;
            screen_y = 0;
            screen_scale_w = SCREEN_W/(float)SCREENWIDTH;
            screen_scale_h = SCREEN_H/(float)SCREENHEIGHT;
            break;
    }
    if (font_tex == NULL) {
        vita2d_init();
        font_tex = vita2d_create_empty_texture(SCREEN_W, SCREEN_H);
        font_data = (uint32_t*) vita2d_texture_get_datap(font_tex);

        pal_tex = vita2d_create_empty_texture_format(SCREENWIDTH, SCREENHEIGHT, SCE_GXM_TEXTURE_FORMAT_P8_1BGR);
        paltex_data = (byte*) vita2d_texture_get_datap(pal_tex);
        palette_data = (uint32_t *)vita2d_texture_get_palette(pal_tex);
    }

    if (bilinear_enabled == true)
    {
        vita2d_texture_set_filters(pal_tex, SCE_GXM_TEXTURE_FILTER_POINT, SCE_GXM_TEXTURE_FILTER_LINEAR);
    }
    else
    {
        vita2d_texture_set_filters(pal_tex, SCE_GXM_TEXTURE_FILTER_POINT, SCE_GXM_TEXTURE_FILTER_POINT);
    }
}

void PSP2_Video_FillScreen(uint32_t size)
{
    if (size <= 0)
    {
        size = SCREEN_W * SCREEN_H * 4;
    }

    memset(font_data, 0x0, size);
}

void PSP2_Video_DrawBuffer()
{
    vita2d_start_drawing();
    vita2d_clear_screen();

    vita2d_draw_texture_scale(pal_tex, screen_x, screen_y, screen_scale_w, screen_scale_h);
    vita2d_draw_texture(font_tex, 0, 0);

    vita2d_end_drawing();
    vita2d_swap_buffers();
}

byte* PSP2_Video_GetVideoPtr()
{
    return paltex_data;
}

uint32_t* PSP2_GetPalettePtr()
{
    return palette_data;
}