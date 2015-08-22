
HOMEBREW_C_SRCS := $(wildcard src/*.c)

TARGET = Vita_Doom
OBJS   = $(patsubst src/%,bin/%,$(HOMEBREW_C_SRCS:.c=.o))

LIBS = -lc_stub -lvita2d -lm_stub -lSceKernel_stub -lSceDisplay_stub -lSceGxm_stub -lSceCtrl_stub -lSceRtc_stub

PREFIX  = $(DEVKITARM)/bin/arm-none-eabi
CC      = $(PREFIX)-gcc
READELF = $(PREFIX)-readelf
OBJDUMP = $(PREFIX)-objdump
CFLAGS = -DPSP2 -specs=psp2.specs
ASFLAGS = $(CFLAGS)

CFLAGS  += -Wall -Ofast -fno-exceptions

all: out/$(TARGET)_fixup.elf

debug: CFLAGS += -DSHOW_DEBUG=1 -DDEBUG 
debug: all

release: CFLAGS += -DRELEASE
release: all

%_fixup.elf: %.elf
	psp2-fixup -q -S $< $@

out/$(TARGET).elf: $(OBJS)
	mkdir -p out
	$(CC) -Wl,-q $(CFLAGS) $^ $(LIBS) -o $@

bin/%.o: src/%.c
	mkdir -p bin
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf out/$(TARGET)_fixup.elf out/$(TARGET).elf bin/$(OBJS)
	@echo "Cleaned"