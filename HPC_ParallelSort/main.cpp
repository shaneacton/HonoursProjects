#include <fstream>
#include <omp.h>

using namespace std;


#include <stdio.h>
#include "sequentialSort.h"
#include "openMPSort.h"


long arrayLengths =20000000;
int experiments = 5;

void testSequential();
void testOpenMP(bool regular);
void testMPI();
void testParallelism();

int main( int argc, const char* argv[] )
{
	testSequential();
    testOpenMP(false);
    OpenMPSort::parallelMethod= "sections";
    testOpenMP(false);

    //testOpenMP(true);
	//testParallelism();
}

void testSequential(){
    printf("testing sequential\n");
	double timeTotal = 0;

    for (int i = 0; i < experiments; ++i){

	    SequentialSort::genArray(arrayLengths,i);
		double start_time = omp_get_wtime();
		SequentialSort::sortArray(0,arrayLengths);
		double time = omp_get_wtime() - start_time;
		timeTotal+=time;

		printf("sorted:%d | " , ArrayUtils::isSorted(SequentialSort::array, SequentialSort::numElements));
	    printf("sequential time:%f\n",time);
	}
	printf("averagetime:%f",(timeTotal/experiments));

}

void testOpenMP(bool regular){
    printf("testing openmp %s | reg: %d\n",OpenMPSort::parallelMethod.c_str() ,regular);

    double timeTotal = 0;

    for (int i = 0; i < experiments; ++i)
    {
		OpenMPSort::genArray(arrayLengths,i);
		double start_time = omp_get_wtime();
		OpenMPSort::sortArray(arrayLengths, regular);
		double time = omp_get_wtime() - start_time;
		timeTotal+=time;
		
		printf("sorted:%d | " , ArrayUtils::isSorted(OpenMPSort::array, OpenMPSort::numElements));
		if(!regular){
	        printf("regular openMP time:%f\n",time);
		}else{
	        printf("openMP time:%f\n",time);
	    }    
	}

	printf("averagetime:%f",(timeTotal/experiments));
	
	
}

void testParallelism(){
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
