#!/bin/bash
#$ -N bmh_mpi_50
#$ -q class8-intel 
#$ -pe mpi 64
#$ -R y

# Module load boost
module load boost/1.57.0

# Module load OpenMPI
module load openmpi-1.8.3/gcc-4.9.2

# Runs a bunch of standard command-line
# utilities, just as an example:

echo "Script began:" `date`
echo "Node:" `hostname`
echo "Current directory: ${PWD}"

echo ""
echo "=== Running 5 trials of Mergesort on 10 million elements ... ==="
for trial in 1 2 3 4 5 ; do
  echo "*** Trial ${trial} ***"
mpirun -np 64 ./bmh_mpi ../gene.txt catcatcatgcatcatcatgcatcatcatgcatcatcatgcatcatcatg
done

echo ""
echo "=== Done! ==="

# eof
