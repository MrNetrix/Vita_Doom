TITLE = Vita_Doom
HOMEBREW_C_SRCS := $(wildcard src/*.c)
HOMEBREW_CPP_SRCS := $(wildcard src/*.cpp)
HOMEBREW_OBJS = $(patsubst src/%,bin/%,$(HOMEBREW_C_SRCS:.c=.o)) $(patsubst src/%,bin/%,$(HOMEBREW_CPP_SRCS:.cpp=.o))

PREFIX = arm-vita-eabi
CC = $(PREFIX)-gcc
CCP = $(PREFIX)-g++
CFLAGS = -Wall -fno-exceptions -Ofast -DPSP2 -mcpu=cortex-a9 -mthumb -mfpu=neon
CPPFLAGS = $(CFLAGS)

all: $(TITLE)

TEST_OUTPUT = bin/*.S out/$(TITLE).elf out/$(TITLE).velf bin/*.o lib/*.a lib/*.o lib/*.S # lib/Makefile
LIBS = -lSceTouch_stub -lSceDisplay_stub -lSceGxm_stub -lSceCtrl_stub -lSceRtc_stub

debug: CFLAGS += -DDEBUG

debugnet: CFLAGS += -DUSE_DEBUGNET
debugnet: LIBS := -ldebugnet -lSceNet_stub -lSceNetCtl_stub $(LIBS)
debugnet: all

.PHONY: $(TITLE)
$(TITLE): out/$(TITLE).elf
	$(PREFIX)-strip -g $<
	vita-elf-create out/$(TITLE).elf out/$(TITLE).velf db.json

out/$(TITLE).elf: $(HOMEBREW_OBJS)
	mkdir -p out
	$(CC) -Wl,-q $(LDFLAGS) $(HOMEBREW_OBJS) -lvita2d -lm $(LIBS) -o $@

bin/%.o: src/%.c
	mkdir -p bin
	$(CC) $(CFLAGS) -c $< -o $@

bin/%.o: src/%.cpp
	mkdir -p bin
	$(CCP) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(ALL_OBJS:.o=.d) $(TARGETS) $(TEST_OUTPUT)
