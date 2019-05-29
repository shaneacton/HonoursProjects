#include "mpi.h"
#include "MPISort.c"
#include <stdlib.h>
#include "arrayUtils.c"
#include "string.h"
#include <omp.h>





long numElements = 10000000;
int experimentNo = 0;

void testMPI();

int main(int argc, char *argv[])
{
	if(argc>1){
		int argElems = atoi(argv[1]);
		numElements = argElems;

		experimentNo = atoi(argv[2]);
	}

	int rc = MPI_Init(&argc, &argv);
    if(rc != MPI_SUCCESS) {
            printf("Error starting MPI program. Terminating. \n");
            MPI_Abort(MPI_COMM_WORLD, rc);
    }


	testMPI();
	//testRegMPI(argc, argv);
}

void testMPI(){
    printf("testing MPI\n" );

    double timeTotal = 0;
   	int *arr;
   	arr= malloc(numElements * sizeof(int));

	generateArray(arr,numElements,experimentNo);		

	double start_time = omp_get_wtime();
	mpiSort(arr, numElements);		
	double time = omp_get_wtime() - start_time;
	timeTotal+=time;

	//printArray(arr,numElements);
	printf("sorted:%d | " , isSorted(arr, numElements));
    printf("regular MPI time:%f\n",time);
    

    MPI_Finalize();
	

	free(arr);


	
	
}