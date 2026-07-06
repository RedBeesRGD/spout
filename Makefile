ifeq ($(strip ${DEVKITPPC}),)
$(error "Please set DEVKITPPC in your environment. export DEVKITPPC=<path to>devkitPPC")
endif

CC=${DEVKITPPC}/bin/powerpc-eabi-gcc
CFLAGS=-O2 -Wall -Wextra -Iinclude
LDFLAGS=-DGEKKO -mcpu=750 -meabi -mhard-float

GC_CFLAGS=-I${DEVKITPRO}/libogc2/gamecube/include -DGAMECUBE_BUILD=1 -DHW_DOL=1
GC_LDFLAGS=-L${DEVKITPRO}/libogc2/gamecube/lib -logc -lm -mogc

WII_CFLAGS=-I${DEVKITPRO}/libogc2/wii/include -DHW_RVL=1
WII_LDFLAGS=-L${DEVKITPRO}/libogc2/wii/lib -lwiiuse -lbte -logc -lm -mrvl

GC_SRC=src/main.c src/utils.c src/console_info.c
WII_SRC=src/main.c src/utils.c src/console_info.c src/externals/libdi.c src/externals/liberror.c src/externals/libgecko.c src/externals/libhaxx.c src/externals/libseeprom.c src/externals/old_utils.c
GC_OBJ=${GC_SRC:src/%.c=build/%.gc.o}
WII_OBJ := $(patsubst src/%.c,build/%.wii.o,$(WII_SRC))

all: gc wii

gc: build/main.gc.dol
wii: build/main.wii.dol

build/main.gc.elf: build ${GC_OBJ}
	${CC} ${GC_OBJ} ${LDFLAGS} ${GC_LDFLAGS} -Wl,-Map,$@.map -o $@

build/main.wii.elf: build ${WII_OBJ}
	${CC} ${WII_OBJ} ${LDFLAGS} ${WII_LDFLAGS} -Wl,-Map,$@.map -o $@

build/%.gc.o: src/%.c
	${CC} ${CFLAGS} ${GC_CFLAGS} -c -o $@ $<

build/%.wii.o: src/%.c
	${CC} ${CFLAGS} ${WII_CFLAGS} -c -o $@ $<

%.dol: %.elf
	elf2dol $< $@

build:
	mkdir -p build
	mkdir -p build/externals

clean:
	rm -rf build

.PHONY: all gc wii clean
