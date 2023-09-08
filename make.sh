#!/bin/sh
set -xe

CC="${CXX:-cc}"
PKGS="sdl2"
CFLAGS="-Wall -Wextra -std=c11 -pedantic -ggdb"
LIBS="-lm -lSDL2_ttf"
SRC="galois.c"
#SRC="test.c"

$CC $CFLAGS `pkg-config --cflags $PKGS` -o galois $SRC $LIBS `pkg-config --libs $PKGS`
