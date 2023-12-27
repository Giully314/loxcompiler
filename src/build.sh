#!/bin/sh

set -xe

CFLAGS="-std=c++20 `llvm-config --cxxflags --ldflags --system-libs --libs core`"

clang++ $CFLAGS -std=c++20 main.cpp scanner.cpp parser.cpp