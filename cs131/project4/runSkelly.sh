#!/bin/bash
#
#$ -cwd
#$ -j y
#$ -S /bin/bash
#$ -M schweer@uci.edu
#$ -pe openmpi 256 
#$ -o ./OutSkelly.out
#
# Use modules to setup the runtime environment
module load sge 
module load gcc/5.2.0
module load openmpi/1.6
#
# Execute the run
#
mpirun -np $NSLOTS ./Skelly 10 1726 
