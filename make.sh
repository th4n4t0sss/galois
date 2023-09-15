#!/bin/sh -xe

CC="${CC:-cc}"
PKGS="sdl2 SDL2_ttf SDL2_gfx"
CFLAGS="-Wall -Wextra -std=c11 -pedantic -ggdb `pkg-config --cflags $PKGS`"
LIBS="`pkg-config --libs $PKGS`"
SRC="galois.c"

$CC $CFLAGS -o galois $SRC $LIBS
