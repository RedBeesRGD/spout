ifeq ($(strip ${DEVKITPPC}),)
$(error "Please set DEVKITPPC in your environment. export DEVKITPPC=<path to>devkitPPC")
endif

CC=${DEVKITPPC}/bin/powerpc-eabi-gcc
CFLAGS=-O2 -Wall -Wextra -Iinclude
LDFLAGS=-DGEKKO -mcpu=750 -meabi -mhard-float

GC_CFLAGS=-I${DEVKITPRO}/libogc2/gamecube/include -DGAMECUBE_BUILD=1
GC_LDFLAGS=-L${DEVKITPRO}/libogc2/gamecube/lib -logc -lm -mogc

WII_CFLAGS=-I${DEVKITPRO}/libogc2/wii/include
WII_LDFLAGS=-L${DEVKITPRO}/libogc2/wii/lib -lwiiuse -lbte -logc -lm -mrvl

SRC=$(wildcard src/*.c)
GC_OBJ=${SRC:src/%.c=build/%.gc.o}
WII_OBJ=${SRC:src/%.c=build/%.wii.o}

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

clean:
	rm -rf build

.PHONY: all gc wii clean
