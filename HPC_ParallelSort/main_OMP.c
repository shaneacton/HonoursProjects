
#include "openMPRegSort.c"
#include "string.h"
#include "arrayUtils.c"
#include <omp.h>


#include <stdlib.h>

long numElements = 10000000;
int experiments =3;

void testRegOpenMP();


int main(int argc, char *argv[])
{
	if(argc>1){
		int argElems = atoi(argv[1]);
		numElements = argElems;
	}

	testRegOpenMP();

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
