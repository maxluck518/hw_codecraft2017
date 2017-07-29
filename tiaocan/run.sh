#!/bin/bash

SCRIPT=$(readlink -f "$0")
BASEDIR=$(dirname "$SCRIPT")
cd $BASEDIR

./SDK-gcc/build.sh
./SDK-gcc/bin/cdn ./debug/2/case$1.txt ./debug/result/result$1.txt 10 50 3 210 > ~/fest.txt
# ./SDK-gcc/bin/cdn ./debug/case_example/case$1.txt ./debug/result/result$1.txt > ~/test.txt
# ./SDK-gcc/bin/cdn ./debug/case_example/case$1.txt ./debug/result/result$1.txt
