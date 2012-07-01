#!/bin/sh

mkdir build && cd $_
GTEST_ROOT=~/Documents/Code/Ruffian/tests/gtest cmake -DLLVM_ROOT=~/Documents/Code/llvm31/build -DCMAKE_USER_MAKE_RULES_OVERRIDE=~/Documents/Code/Ruffian/ClangOverrides.txt -DCMAKE_C_COMPILER="/usr/local/bin/clang" -DCMAKE_CXX_COMPILER="/usr/local/bin/clang++" ..

