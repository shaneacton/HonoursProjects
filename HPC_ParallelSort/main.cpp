#include <fstream>
#include <omp.h>

using namespace std;


#include <stdio.h>
#include "sequentialSort.h"


long arrayLengths =10000000;
void testSequential();
void testOpenMP();
void testMPI();

int main( int argc, const char* argv[] )
{
	testSequential();
    

}

void testSequential(){
	SequentialSort::genArray(arrayLengths);
	double start_time = omp_get_wtime();
	SequentialSort::sortArray(0,arrayLengths);
	double time = omp_get_wtime() - start_time;
	//SequentialSort::printArray();

	printf("sorted:%d | " , ArrayUtils::isSorted(SequentialSort::array, SequentialSort::numElements));
    printf("sequential time:%f\n",time);

}