
#include "mpi.h"
#include "MPIRegSort.c"
#include <stdlib.h>
#include "arrayUtils.c"
#include "string.h"
#include <omp.h>



long numElements = 100000000;
int experimentNo = 0;


void testRegMPI();

int main(int argc, char *argv[])
{
	//printf("main hello\n");

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


	testRegMPI();
}

void testRegMPI(){

   	int *arr;
   	arr= malloc(numElements * sizeof(int));

   	int myId;
    MPI_Comm_rank(MPI_COMM_WORLD,&myId);

    if(myId == 0 && experimentNo == 0){
        //printf("testing regular MPI, length:%ld\n", numElements );
    }



	generateArray(arr,numElements,experimentNo);		
	double start_time = omp_get_wtime();
	psrs_mpi(arr, numElements);		
	double time = omp_get_wtime() - start_time;

	//printArray(arr,numElements);

	if(myId == 0){
		if(!isSorted(arr, numElements)){
			printf("WARNING!!! array not sorted\n");
		}
	    printf("%f\n",time);
	}  

	free(arr);
	MPI_Finalize();

	
	
}