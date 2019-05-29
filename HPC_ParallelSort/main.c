
#include "openMPRegSort.c"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "arrayUtils.c"

#include "mpi.h"

long numElements = 50000000;
int experiments =5;

void testRegOpenMP();
void testRegMPI();

int main(int argc, char *argv[])
{
	MPI_Init(&argc,&argv);
	testRegOpenMP();
	testRegOpenMP();
}

void testRegOpenMP(){
    printf("testing regular openmp\n" );

    double timeTotal = 0;
   	int *arr;
   	arr= malloc(numElements * sizeof(int));

    for (int i = 0; i < experiments; ++i)
    {
		generateArray(arr,numElements,i);		
		double start_time = omp_get_wtime();
		psrs_sort(arr, numElements);		
		double time = omp_get_wtime() - start_time;
		timeTotal+=time;

		//printArray(arr,numElements);
		printf("sorted:%d | " , isSorted(arr, numElements));
	    printf("regular openMP time:%f\n",time);
	    
	}

	printf("averagetime:%f\n\n",(timeTotal/experiments));
	
	
}

void testRegOpenMP(){
    printf("testing regular MPI\n" );

    double timeTotal = 0;
   	int *arr;
   	arr= malloc(numElements * sizeof(int));

    for (int i = 0; i < experiments; ++i)
    {
		generateArray(arr,numElements,i);		
		double start_time = omp_get_wtime();
		psrs_mpi(arr, numElements);		
		double time = omp_get_wtime() - start_time;
		timeTotal+=time;

		//printArray(arr,numElements);
		printf("sorted:%d | " , isSorted(arr, numElements));
	    printf("regular MPI time:%f\n",time);
	    
	}

	printf("averagetime:%f\n",(timeTotal/experiments));
	
	
}