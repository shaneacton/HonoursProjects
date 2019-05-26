# Declare some variables to help construct our compile command
CC=g++         # the compiler
LIBS=-lm       # the libraries we will ref

main: main.o 
	$(CC) -o main main.o sequentialSort.h -std=c++11

main.o: main.cpp sequentialSort.h arrayUtils.cpp
	$(CC) -c main.cpp sequentialSort.h -std=c++11

