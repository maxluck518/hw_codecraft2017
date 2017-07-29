#!/bin/bash

SCRIPT=$(readlink -f "$0")
BASEDIR=$(dirname "$SCRIPT")
cd $BASEDIR

./SDK-gcc/build.sh
./SDK-gcc/bin/cdn ./debug/0/case$1.txt ./debug/result/result$1.txt > ~/fest.txt
# ./SDK-gcc/bin/cdn ./debug/case_example/case$1.txt ./debug/result/result$1.txt > ~/test.txt
# ./SDK-gcc/bin/cdn ./debug/case_example/case$1.txt ./debug/result/result$1.txt
