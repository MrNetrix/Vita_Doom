# Makefile.in for Doom-PSP 
 
TARGET = Doom-PSP
OBJS    = am_map.o d_items.o d_main.o d_net.o doomdef.o doomstat.o dstrings.o \
          f_finale.o f_wipe.o g_game.o hu_lib.o \
	  hu_stuff.o i_main.o i_net.o i_sound.o i_system.o i_video.o disablefpuexceptions.o \
	  info.o m_argv.o m_bbox.o m_cheat.o m_fixed.o m_menu.o m_misc.o m_random.o m_swap.o \
	  p_ceilng.o p_doors.o p_enemy.o p_floor.o p_inter.o p_lights.o p_plats.o p_map.o \
	  p_maputl.o p_mobj.o  p_saveg.o p_pspr.o p_saveg.o p_setup.o p_sight.o p_spec.o \
	  p_switch.o p_telept.o p_tick.o p_user.o r_bsp.o r_data.o r_draw.o r_main.o r_plane.o \
	  r_segs.o r_sky.o r_things.o s_sound.o sounds.o st_lib.o st_stuff.o tables.o v_video.o \
	  w_wad.o wi_stuff.o z_zone.o pspglue.o


INCDIR = 
CFLAGS = -O2 -G0 -Wall -DPSP
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)
USE_PSPSDK_LIBC = 1

LIBDIR = 
LDFLAGS =
LIBS =  -lpspgu -lpspaudiolib -lpspaudio -lm -lpsppower

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Doom-PSP V0.04 S.S. b5
PSP_EBOOT_ICON = ICON0.PNG

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak


 
