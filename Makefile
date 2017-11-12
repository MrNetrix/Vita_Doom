PROJECT = Vita_Doom
PROJECT_TITLE := Vita Doom
PROJECT_TITLEID := DOOM00666

HOMEBREW_C_SRCS := $(wildcard src/*.c)
HOMEBREW_CPP_SRCS := $(wildcard src/*.cpp)
HOMEBREW_OBJS = $(patsubst src/%,bin/%,$(HOMEBREW_C_SRCS:.c=.o)) $(patsubst src/%,bin/%,$(HOMEBREW_CPP_SRCS:.cpp=.o))

PREFIX = arm-vita-eabi
CC = $(PREFIX)-gcc
CCP = $(PREFIX)-g++
STRIP := $(PREFIX)-strip
CFLAGS = -Wall -fno-exceptions -DPSP2 -mcpu=cortex-a9 -mthumb -mfpu=neon
CPPFLAGS = $(CFLAGS)

all: package

package: $(PROJECT).vpk

$(PROJECT).vpk: eboot.bin param.sfo
	vita-pack-vpk -s out/param.sfo -b out/eboot.bin \
		--add sce_sys/icon0.png=sce_sys/icon0.png \
		--add sce_sys/livearea/contents/bg.png=sce_sys/livearea/contents/bg.png \
		--add sce_sys/livearea/contents/startup.png=sce_sys/livearea/contents/startup.png \
		--add sce_sys/livearea/contents/template.xml=sce_sys/livearea/contents/template.xml \
	out/$(PROJECT).vpk

TEST_OUTPUT = bin/*.S out/$(PROJECT).elf out/$(PROJECT).velf bin/*.o lib/*.a lib/*.o lib/*.S # lib/Makefile
LIBS = -lvita2d -lm -lSceLibKernel_stub -lSceTouch_stub -lSceDisplay_stub -lSceGxm_stub -lSceCtrl_stub -lSceRtc_stub -lScePower_stub -lSceSysmodule_stub -lSceCommonDialog_stub -lSceAudio_stub


debugnet: CFLAGS += -DUSE_DEBUGNET -g
debugnet: LIBS := -ldebugnet -lSceNet_stub -lSceNetCtl_stub $(LIBS)
debugnet: all

eboot.bin: out/$(PROJECT).velf
	vita-make-fself -s out/$(PROJECT).velf out/eboot.bin

param.sfo:
	mkdir out || true
	vita-mksfoex -s TITLE_ID="$(PROJECT_TITLEID)" "$(PROJECT_TITLE)" out/param.sfo
	
out/$(PROJECT).velf: out/$(PROJECT).elf
	#$(STRIP) -g $<
	vita-elf-create $< $@

out/$(PROJECT).elf: $(HOMEBREW_OBJS)
	mkdir -p out
	$(CCP) -Wl,-q $(LDFLAGS) $(HOMEBREW_OBJS) $(LIBS) -o $@

bin/%.o: src/%.c
	mkdir -p bin
	$(CC) $(CFLAGS) -c $< -o $@

bin/%.o: src/%.cpp
	mkdir -p bin
	$(CCP) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(PROJECT).velf $(PROJECT).elf $(PROJECT).vpk param.sfo eboot.bin $(ALL_OBJS:.o=.d) $(TARGETS) $(TEST_OUTPUT)
	rm -rf bin out

rebuilddebugnet: clean
rebuilddebugnet: debugnet
	
rebuild: clean
rebuild: all
