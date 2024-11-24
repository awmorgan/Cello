#!/bin/bash
exec > >(tee -i make.log) 2>&1

set -x #print commands as they are executed
set -e #exit when any command fails

# Default to release if no argument is provided
BUILD_TYPE=${1:-release}

# Set optimization flag and sanitizer flag based on build type
if [ "$BUILD_TYPE" == "debug" ]; then
        OPT_FLAG="-O0"
        SANITIZE_FLAGS="-fsanitize=address -fsanitize=undefined"
        DEBUG_FLAGS="-DDEBUG=1 -ggdb"
else
        OPT_FLAG="-O3"
        SANITIZE_FLAGS="-fsanitize=undefined"
        DEBUG_FLAGS=""
fi

WARN_FLAGS="\
-Wall -Wextra -Wpedantic \
-Werror \
-Wno-dollar-in-identifier-extension \
-Wno-newline-eof \
"

CELLO_FLAGS="\
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

clang $OPT_FLAG \
    $DEBUG_FLAGS \
    $WARN_FLAGS \
    $CELLO_FLAGS \
    $SANITIZE_FLAGS \
    cello_tests.c \
    -o build/main

