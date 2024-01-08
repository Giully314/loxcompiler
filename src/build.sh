#!/bin/sh

set -xe

LFLAGS="`llvm-config --cxxflags --ldflags --system-libs --libs core`"
CFILES="main.cpp scanner.cpp parser.cpp types.cpp token.cpp node.cpp ast_printer.cpp"
TEMP="llvm_visitor.cpp  -Wall -Wextra -Wconversion -Wpedantic -Werror"
CFLAGS="-std=c++20 -fexceptions"

clang++  $CFLAGS $CFILES -o lox