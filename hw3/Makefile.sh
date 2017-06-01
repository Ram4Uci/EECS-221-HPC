#!/bin/bash
echo "compiling"
module load cuda/5.0
module load gcc/4.4.3
nvcc $1 timer.c -o $2