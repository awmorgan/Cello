#!/bin/bash
exec > >(tee -i make.log) 2>&1

set -x #print commands as they are executed
set -e #exit when any command fails

# Initialize flags
FLAGS=""
OPTIMIZATION_SET=false

# Parse command line arguments
for arg in "$@"; do
    case $arg in
    debug)
        FLAGS+=" -O0 -ggdb"
        OPTIMIZATION_SET=true
        ;;
    release)
        FLAGS+=" -O3"
        OPTIMIZATION_SET=true
        ;;
    ubsan)
        FLAGS+=" -fsanitize=undefined"
        ;;
    asan)
        FLAGS+=" -fsanitize=address"
        ;;
    clean)
        echo "Cleaning build artifacts..."
        rm -f test make.log
        ;;
    *)
        echo "Unknown option: $arg"
        echo "Usage: $0 [debug|release|ubsan|asan|clean]"
        exit 1
        ;;
    esac
done

if [ "$OPTIMIZATION_SET" = false ]; then
    FLAGS+=" -O3"
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

clang \
    $FLAGS \
    cello_tests.c \
    -o test
