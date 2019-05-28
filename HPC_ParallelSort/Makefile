# Declare some variables to help construct our compile command
CC=g++         # the compiler
LIBS=-lm       # the libraries we will ref

main: main.cpp sequentialSort.h openMPSort.h
	$(CC) -o main -fopenmp main.cpp -std=c++11

clean:
    rm main && rm *.o && rm *.gch

