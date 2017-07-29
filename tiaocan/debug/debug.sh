#!/bin/bash

SCRIPT=$(readlink -f "$0")
BASEDIR=$(dirname "$SCRIPT")
cd $BASEDIR

./SDK-gcc/build.sh
# cd ./SDK-gcc/cdn
# gdb --args ../bin/cdn ../../case_example/case$1.txt ../../result/result$1.txt
./SDK-gcc/bin/cdn ./debug/case_example/case$1.txt ./debug/result/result$1.txt > ~/test.txt
