# Declare some variables to help construct our compile command
CC=g++         # the compiler
LIBS=-lm       # the libraries we will ref

main_cpp: main.cpp sequentialSort.h openMPSort.h arrayUtils.cpp main_c
	$(CC) -o main_cpp -fopenmp main.cpp -std=c++11

main_c: main.c openMPRegSort.c arrayUtils.c
	gcc -o main_c -fopenmp main.c --std=c11 -lm

clean:
	rm main_cpp
	rm main_c

run:
	./main_cpp
	./main_c
