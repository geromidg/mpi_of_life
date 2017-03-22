#!/bin/bash

FILE_NAME=results.txt

make -C src/ clean
make -C src/

[ -e $FILE_NAME ] && rm $FILE_NAME

for processes in 1 2 4	
do
    echo "Running with $processes processes..."
    echo -n "$processes " >> $FILE_NAME
    mpirun -np $processes src/game_of_life >> $FILE_NAME
done
