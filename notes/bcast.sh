#!/bin/bash
#$ -N TEST
#$ -q class8-intel
#$ -pe mpi 48
#$ -R y

# Grid Engine Notes:
# -----------------
# 1) Use "-R y" to request job reservation otherwise single 1-core jobs
#    may prevent this multicore MPI job from running.   This is called
#    job starvation.

# Module load OpenMPI
module load openmpi-1.8.3/gcc-4.8.3

# Run the program with the ouput going to file out
mpirun -np 48  ./compare_bcast 100000 5
