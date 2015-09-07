#!/bin/bash
while true ; do            T=$(time g++ --std=c++11 -O3 a.cpp 2>&1) && clear ; echo $T && { echo "Compile: $T" ; echo " " ; time ./a.out ; } ; sleep 0.03 ; done 

