#!/bin/bash

rat=(50 70 90)
thd=(1 2 4 8 16 32)
dis=(0 1)

for r in ${rat[@]}; do
    for t in ${thd[@]}; do
        for d in ${dis[@]}; do
            ./bench $t $r $d
        done
    done
done
