
#include <string>
#include <sstream>

#include <stdlib.h>

int maxValue = 20000;
int seed  =10;

namespace ArrayUtils {

	int* arr;

	int* generateArray(int length){
		srand(seed);

		arr = new int[length];

		for (int i = 0; i < length; ++i)
		{
			arr[i]=rand() % maxValue + 1;
		}

		printf("creating new array of length %d and populating it\n",length);

		return arr;
	}

	void printArray(int* array, int length){

		printf("%d~[",length);

		for (int i = 0; i < length; ++i)
		{
			printf("%d,",array[i]);
		}

		printf("]\n");
	}

	

}