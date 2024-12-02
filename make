#!/bin/bash
exec > >(tee -i make.log) 2>&1

set -e #exit when any command fails

# Initialize flags
FLAGS="
-D_CRT_SECURE_NO_WARNINGS=1 \
-Iinclude \
-ggdb \
-fuse-ld=lld \
"

OPTIMIZATION_SET=false
RUN_TESTS=false

# Parse command line arguments
for arg in "$@"; do
    case $arg in
    debug)
        FLAGS+=" -O0"
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
    werror)
        FLAGS+=" -Werror"
        ;;
    cov)
        FLAGS+=" -fprofile-instr-generate -fcoverage-mapping"
        ;;
    clean)
        echo "Cleaning build artifacts..."
        set -x
        rm -f test make.log
        set +x
        ;;
    runtests)
        RUN_TESTS=true
        ;;
    *)
        echo "Unknown option: $arg"
        echo "Usage: $0 [debug|release|ubsan|asan|werror|clean|runtests]"
        exit 1
        ;;
    esac
done

if [ "$OPTIMIZATION_SET" = false ]; then
    FLAGS+=" -O3"
fi

CELLO_FLAGS=" 
$FLAGS \
-Wall -Wextra \
-Wno-unused-variable \
-Wno-unused-parameter \
-Wno-missing-field-initializers \
-Wno-unused-function \
-Wno-sign-compare \
"

set -x
clang $CELLO_FLAGS -c cello.c
clang $FLAGS tests/ptest.c tests/test.c cello.o -o test.exe
set +x

if [ "$RUN_TESTS" = true ]; then
  set -x
  ./test.exe
  set +x
fi


