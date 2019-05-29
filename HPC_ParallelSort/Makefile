# Declare some variables to help construct our compile command
CC=g++         # the compiler
LIBS=-lm       # the libraries we will ref

main_cpp: main.cpp sequentialSort.h openMPSort.h arrayUtils.cpp 
	$(CC) -o main_cpp -fopenmp  main.cpp -std=c++11

main_OMP: main_OMP.c openMPRegSort.c arrayUtils.c
	gcc -o main_OMP -fopenmp main_OMP.c --std=c11 -lm

main_MPI: main_MPI.c  arrayUtils.c MPISort.c
	mpicc -o main_MPI -fopenmp main_MPI.c --std=c11 -lm

main_MPIReg: main_MPIReg.c arrayUtils.c MPIRegSort.c
	mpicc -o main_MPIReg -fopenmp main_MPIReg.c --std=c11 -lm

clean:
	rm main_cpp
	rm main_OMP
	rm main_MPI
	rm main_MPIReg

run:
	./main_cpp
	./main_c
