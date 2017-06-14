#!/bin/bash
#$ -N bmh_s
#$ -q class8-intel 
#$ -pe openmp 1

# Module load gcc compiler version 4.9.2
module load  gcc/4.9.2

# Runs a bunch of standard command-line
# utilities, just as an example:

echo "Script began:" `date`
echo "Node:" `hostname`
echo "Current directory: ${PWD}"

echo ""
echo "=== Running 5 trials of Mergesort on 10 million elements ... ==="
for trial in 1 2 3 4 5 ; do
  echo "*** Trial ${trial} ***"
  ./bmh_s ../gene.txt cat
  ./bmh_s ../gene.txt catgtag 
  ./bmh_s ../gene.txt catcatcatg
  ./bmh_s ../gene.txt catcatcatgcatcatcatg
  ./bmh_s ../gene.txt catcatcatgcatcatcatgcatcatcatgcatcatcatgcatcatcatg
done

echo ""
echo "=== Done! ==="

# eof
