
#include "openMPRegSort.c"
//#include "stdio.h"
#include "string.h"
//#include "stdlib.h"
#include "arrayUtils.c"

#include "MPIRegSort.c"
#include "MPISort.c"
//#include <stdlib.h>
//#include <sys/time.h>
//#include <unistd.h>
#include "mpi.h"

long numElements = 10000000;
int experiments =2;

void testRegOpenMP();
void testMPI();
void testRegMPI();


int main(int argc, char *argv[])
{
	if(argc>1){
		//printf("c received args: %s\n", argv[1]);
		int argElems = atoi(argv[1]);
		//printf("ArrayLength = %d\n", argElems);
		numElements = argElems;
	}

	int rc = MPI_Init(&argc, &argv);
    if(rc != MPI_SUCCESS) {
            printf("Error starting MPI program. Terminating. \n");
            MPI_Abort(MPI_COMM_WORLD, rc);
    }	
    printf("initialised MPI\n");


	testRegOpenMP();
	testMPI();
	testRegMPI();
}

void testRegOpenMP(){
    printf("testing regular openmp, num experiments:%d\n", experiments );

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

void testRegMPI(){
    printf("testing regular MPI\n" );

    double timeTotal = 0;
   	int *arr;
   	arr= malloc(numElements * sizeof(int));

    for (int i = 0; i < experiments; ++i)
    {
    	//printf("experiment %d\n",i);
		generateArray(arr,numElements,i);		
		double start_time = omp_get_wtime();
		psrs_mpi(arr, numElements);		
		double time = omp_get_wtime() - start_time;
		timeTotal+=time;

		//printArray(arr,numElements);
		printf("sorted:%d | " , isSorted(arr, numElements));
	    printf("regular MPI time:%f\n",time);
	    
	}
	MPI_Finalize();
	free(arr);

	printf("averagetime:%f\n",(timeTotal/experiments));
	
	
}

void testMPI(){
    printf("testing MPI\n" );

    double timeTotal = 0;
   	int *arr;
   	arr= malloc(numElements * sizeof(int));

    for (int i = 0; i < experiments; ++i)
    {
    	//printf("experiment %d\n",i);
		generateArray(arr,numElements,i);		

		//printArray(arr,numElements);


		double start_time = omp_get_wtime();
		mpiSort(arr, numElements);		
		double time = omp_get_wtime() - start_time;
		timeTotal+=time;

		//printArray(arr,numElements);
		printf("sorted:%d | " , isSorted(arr, numElements));
	    printf("regular MPI time:%f\n",time);
	    
	}
	free(arr);

	printf("averagetime:%f\n\n",(timeTotal/experiments));
	
	
}