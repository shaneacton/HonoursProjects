#include <fstream>
#include <omp.h>

using namespace std;


#include <stdio.h>
#include "sequentialSort.h"
#include "openMPSort.h"


long arrayLengths =10000000;
int experiments = 10;
bool testSeq = true;

void testSequential();
void testOpenMP();
void testParallelism();

int main( int argc, const char* argv[])
{
	if(argc>1){
		int argElems = std::stoi(argv[1]);
		printf("###################################################\n##\t\tArrayLength = %d\t\t##\n###################################################\n", argElems);
		arrayLengths = argElems;

		if(argc >2){
			testSeq = (std::stoi(argv[2]) == 1);
		}
	}

	if(testSeq){
		testSequential();
	}
	

	printf("\nUsing Parallel. threads=%d\n\n",omp_get_max_threads());
    testOpenMP();
	//testParallelism();
}

void testSequential(){
    printf("*sequential\n");
	double timeTotal = 0;

    for (int i = 0; i < experiments; ++i){

	    SequentialSort::genArray(arrayLengths,i);
		double start_time = omp_get_wtime();
		SequentialSort::sortArray(0,arrayLengths);
		double time = omp_get_wtime() - start_time;
		timeTotal+=time;

		//printf("sorted:%d | " , ArrayUtils::isSorted(SequentialSort::array, SequentialSort::numElements));
	    //printf("sequential time:%f\n",time);
	}
	printf("%f\n",(timeTotal/experiments));

}

void testOpenMP(){
    printf("*openmp\n" );

    double timeTotal = 0;

    for (int i = 0; i < experiments; ++i)
    {
		OpenMPSort::genArray(arrayLengths,i);
		double start_time = omp_get_wtime();
		OpenMPSort::sortArray(arrayLengths);
		double time = omp_get_wtime() - start_time;
		timeTotal+=time;

		//printf("sorted:%d | " , ArrayUtils::isSorted(OpenMPSort::array, OpenMPSort::numElements));
	    //printf("openMP time:%f\n",time);
	    
	}

	printf("%f\n\n",(timeTotal/experiments));
	
	
}

void testParallelism(){
	//we expect interleavings of car and race
	#pragma omp parallel 
    { 
        #pragma omp single nowait
	     { 
			printf("A "); 
			int c =100;

			#pragma omp task 
			{
				for (int i = 0; i < c; ++i)
				{
					printf("race %d",omp_get_thread_num());
				}
			} 

			#pragma omp task 
			{
			
				for (int i = 0; i < c; ++i){
					printf("car %d",omp_get_thread_num());
				}

			} 
		 } 
	} // End of parallel region 
	printf("\n"); 
}
