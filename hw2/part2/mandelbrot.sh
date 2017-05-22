#!/bin/bash
#$ -N Mandelbrot
#$ -q class64-amd
#$ -pe one-node-mpi 32
#$ -R y

# Grid Engine Notes:
# -----------------
# 1) Use "-R y" to request job reservation otherwise single 1-core jobs
#    may prevent this multicore MPI job from running.   This is called
#    job starvation.

# Module load boost
module load boost/1.57.0

# Module load OpenMPI
module load openmpi-1.8.3/gcc-4.9.2

# Run the program 
for size in 500 1000 2000 5000 10000 20000 30000; do
    echo "**** Size = ${size} ****"
    echo "**** Master Slave Program ****"
    mpirun -np 32  ./mandelbrot_ms ${size} ${size}
    echo "**** Joe's Program ****"
    mpirun -np 32  ./mandelbrot_joe ${size} ${size}
    echo "**** Susie's Program ****"
    mpirun -np 32  ./mandelbrot_susie ${size} ${size}
    echo "**** END ****"
done

    
