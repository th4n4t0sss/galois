#!/bin/sh -ex

CC="${CC:-cc}"
PKGS="sdl2 SDL2_ttf SDL2_gfx"
CFLAGS="-Wall -Wextra -std=c11 -pedantic -ggdb `pkg-config --cflags $PKGS`"
LIBS="-lm `pkg-config --libs $PKGS`"
SRC="galois.c"

$CC $CFLAGS -o galois $SRC $LIBS
