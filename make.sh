#!/bin/bash
exec > >(tee -i make.log) 2>&1

set -x #print commands as they are executed
set -e #exit when any command fails

# Default to release if no argument is provided
BUILD_TYPE=${1:-release}

# Set optimization flag and sanitizer flag based on build type
if [ "$BUILD_TYPE" == "debug" ]; then
        FLAGS="-O0 -ggdb"
        SANITIZE_FLAGS="-fsanitize=address -fsanitize=undefined"
else
        OPT_FLAG="-O3"
        SANITIZE_FLAGS="-fsanitize=undefined"
fi

FLAGS+=" \
-Wall -Wextra -Wpedantic \
-Werror \
-Wno-dollar-in-identifier-extension \
-Wno-newline-eof \
-Wno-gnu-zero-variadic-macro-arguments \
-Wno-c23-extensions \
-Wno-unused-variable \
-Wno-unused-parameter \
-Wno-extra-semi \
-Wno-missing-field-initializers \
-Wno-unused-function \
-Wno-sign-compare \
-D_CRT_SECURE_NO_WARNINGS \
"

rm -rf build
mkdir -p build

clang \
    $FLAGS \
    $SANITIZE_FLAGS \
    cello_tests.c \
    -o build/main

