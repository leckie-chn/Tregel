#!/bin/bash

for i in 0 1 2 3 4 5 6 7 8 9
do
    ~/build/worker ~/Tregel/conf/worker${i}.xml > ~/Tregel/logs/worker${i}.log 2>&1 &
done
