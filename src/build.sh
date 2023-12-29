#!/bin/sh

set -xe

CFLAGS="`llvm-config --cxxflags --ldflags --system-libs --libs core`"
CFILES="main.cpp scanner.cpp parser.cpp types.cpp token.cpp node.cpp ast_printer.cpp"
TEMP="llvm_visitor.cpp"


clang++ $CFLAGS -std=c++20 $CFILES -o lox