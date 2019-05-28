#include <fstream>
#include <omp.h>

using namespace std;


#include <stdio.h>
#include "sequentialSort.h"
#include "openMPSort.h"


long arrayLengths =128;
void testSequential();
void testOpenMP();
void testMPI();
void testParallelism();

int main( int argc, const char* argv[] )
{
	//testSequential();
    //testOpenMP();
	testParallelism();
}

void testSequential(){
	SequentialSort::genArray(arrayLengths);
	double start_time = omp_get_wtime();
	SequentialSort::sortArray(0,arrayLengths);
	double time = omp_get_wtime() - start_time;
	

	printf("sorted:%d | " , ArrayUtils::isSorted(SequentialSort::array, SequentialSort::numElements));
    printf("sequential time:%f\n",time);

}

void testOpenMP(){
	OpenMPSort::genArray(arrayLengths);
	double start_time = omp_get_wtime();
	OpenMPSort::sortArray(arrayLengths);
	double time = omp_get_wtime() - start_time;

	printf("sorted:%d | " , ArrayUtils::isSorted(OpenMPSort::array, OpenMPSort::numElements));
    printf("openMP time:%f\n",time);
}

void testParallelism(){
	#pragma omp parallel 
    { 
        #pragma omp single nowait
	     { 
			printf("A "); 

			#pragma omp task 
			{
			printf("race %d",omp_get_thread_num());
			printf("race %d",omp_get_thread_num());
			printf("race %d",omp_get_thread_num());
			printf("race %d",omp_get_thread_num());
			} 
			#pragma omp task 
			{
			printf("car %d",omp_get_thread_num());
			printf("car %d",omp_get_thread_num());
			printf("car %d",omp_get_thread_num());
			printf("car %d",omp_get_thread_num());
			} 
		 } 
	} // End of parallel region 
	printf("\n"); 
}
