#!/bin/bash
###########################################################################################

# The line below indicates which accounting group to log your job against - must be its
#SBATCH --account=icts

# The line below selects the group of nodes you require - you will all run on curie
#SBATCH --partition=curie

# The line below means you need 1 worker node and a total of 2 cores
#SBATCH --nodes=2 --ntasks=8

# The line below indicates the wall time your job will need, 10 hours for example. NB, this is a mandatory directive!
# Note that 10:00 is 10 minutes
#SBATCH --time=60:00

# The line below means your job will be canceled if it consumes more than 4GB of RAM per requested core. Max 9000.

# A sensible name for your job, try to keep it short
#SBATCH --job-name="ShaneHPC"

#Modify the lines below for email alerts. Valid type values are NONE, BEGIN, END, FAIL, REQUEUE, ALL 
#SBATCH --mail-user=ACTSHA001@myuct.ac.za
#SBATCH --mail-type=NONE

# NB, for more information read https://computing.llnl.gov/linux/slurm/sbatch.html

# Use module to gain easy access to software, typing module avail lists all packages.
numThreads=8;

module load mpi/openmpi-4.0.1


make main_cpp
make main_OMP
make main_MPI
make main_MPIReg

for threadCount in $( seq 7 $numThreads ); do
	if [ $((threadCount%2)) == 0 -a $threadCount != 6 ];
	#if [ $threadCount != 6 ];
	then
		#echo 'threads = ' $threadCount;
		export OMP_NUM_THREADS=$threadCount
		numElements=12500000;


		if [ $threadCount == 2 ];
		then
			./main_cpp $numElements 1
		fi;

		if [ $threadCount != 2 ];
		then
			./main_cpp $numElements 0
		fi;

		./main_OMP $numElements

		echo "*MPI"

		for experiment in {0..9}; do
			mpirun -np $threadCount --mca btl_openib_warn_nonexistent_if 0 --quiet main_MPI $numElements $experiment

		done;

		echo "*MPI Regular"

		for experiment in {0..9}; do
			mpirun -np $threadCount --mca btl_openib_warn_nonexistent_if 0 --quiet main_MPIReg $numElements $experiment

		done;



	fi;
done;

echo "experiments complete"
