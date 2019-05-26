
#include <string>
#include <sstream>

#include <stdlib.h>

int maxValue = 2000000;

namespace ArrayUtils {

	int* arr;

	int* generateArray(long length){
		srand(length);

		arr = new int[length];

		for (long i = 0; i < length; ++i)
		{
			arr[i]=rand() % maxValue + 1;
		}

		printf("creating new array of length %ld and populating it\n",length);

		return arr;
	}

	void printArray(int* array, long length){

		printf("%ld~[",length);

		for (int i = 0; i < length; ++i)
		{
			printf("%d,",array[i]);
		}

		printf("]\n");
	}

	bool isSorted(int* array, long length){
		for (long i = 0; i < length-1; ++i)
		{
			if(array[i] > array[i+1]){
				return false;
			}
		}

		return true;
	}

}