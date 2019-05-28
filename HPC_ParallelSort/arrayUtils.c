
#ifndef ARRAYUTILS_CPP
#define ARRAYUTILS_CPP

#include "stdio.h"
#include "string.h"
#include "stdlib.h"

int maxValue = 20000;


void generateArray(int* emptyArray, long length, int seed){
	srand(length*seed);


	for (long i = 0; i < length; ++i)
	{
		emptyArray[i]=rand() % maxValue + 1;
	}

	//printf("creating new array of length %ld and populating it\n",length);
}

void printArray(int* array, long length){

	printf("%ld~[",length);

	for (int i = 0; i < length; ++i)
	{
		printf("%d,",array[i]);
	}

	printf("]\n");
}

int isSorted(int* array, long length){
	for (long i = 0; i < length-1; ++i)
	{
		if(array[i] > array[i+1]){
			return 0;
		}
	}

	return 1;
}



#endif