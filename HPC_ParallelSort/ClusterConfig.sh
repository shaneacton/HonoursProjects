#!/bin/sh
###########################################################################################

# The line below indicates which accounting group to log your job against - must be its
#SBATCH --account=icts

# The line below selects the group of nodes you require - you will all run on curie
#SBATCH --partition=curie

# The line below means you need 1 worker node and a total of 2 cores
#SBATCH --nodes=1 --ntasks=8

# The line below indicates the wall time your job will need, 10 hours for example. NB, this is a mandatory directive!
# Note that 10:00 is 10 minutes
#SBATCH --time=10:00

# The line below means your job will be canceled if it consumes more than 4GB of RAM per requested core. Max 9000.

# A sensible name for your job, try to keep it short
#SBATCH --job-name="ShaneHPC"

#Modify the lines below for email alerts. Valid type values are NONE, BEGIN, END, FAIL, REQUEUE, ALL 
#SBATCH --mail-user=ACTSHA001@myuct.ac.za
#SBATCH --mail-type=NONE

# NB, for more information read https://computing.llnl.gov/linux/slurm/sbatch.html

# Use module to gain easy access to software, typing module avail lists all packages.
module load mpi/openmpi-4.0.1

make
make main_c

./main_cpp 1000
./main_c 1000

./main_cpp 10000
./main_c 10000

#./main_cpp 100000000
#./main_c 100000000