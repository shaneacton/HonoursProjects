#include <fstream>
#include <omp.h>

using namespace std;


#include <stdio.h>
#include "sequentialSort.h"


long arrayLengths =10000000;


int main( int argc, const char* argv[] )
{
	// Prints each argument on the command line.
	for( int i = 0; i < argc; i++ )
	{
		printf( "arg %d: %s\n", i, argv[i] );
	}

	SequentialSort::genArray(arrayLengths);
	double start_time = omp_get_wtime();
	SequentialSort::sortArray(0,arrayLengths);
	double time = omp_get_wtime() - start_time;
	SequentialSort::printArray();
    printf("sorted:%d\n" , ArrayUtils::isSorted(SequentialSort::array, SequentialSort::numElements));
    printf("sequential time:%f\n",time);

}