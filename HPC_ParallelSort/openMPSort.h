/**
 sequentialSort.h file:
 Shane Acton
 ACTSHA001
 */
#ifndef OMPSORT_H
#define OMPSORT_H

#include "arrayUtils.cpp"

#include <string>
#include <sstream>
#include <omp.h>
#include <cmath>

namespace OpenMPSort {

  long numElements;
  int* array;
  int numThreads;

  void sortArrayInternal(int start,int end,int level, std::string pathID);

  void genArray(long nElements){
    numElements = nElements;
    //printf("generating array of length %ld\n", numElements);
    array = ArrayUtils::generateArray(nElements);
    //ArrayUtils::printArray(array, numElements);
  
    //printf("sorted:%d\n" , ArrayUtils::isSorted(array, numElements));
  }



  void sortArray(int length){
    numThreads= omp_get_max_threads(); 
    printf("omp sorting using %d threads\n",numThreads);
    
    #pragma omp parallel
    //#pragma omp parallel num_threads(numThreads)
    {

        #pragma omp single// nowait
    	{
    		sortArrayInternal(0,length, 0, "");	
    	}
    }

  }

  void sortArrayInternal(int start,int end,int level, std::string pathID){
    printf("%s, %d\n", pathID.c_str(), omp_get_thread_num());
  	int length = end-start;
    if(length<= 1){
      //printf("reached base, start:%d end:%d\n",start,end);
      return;
    }

    
    int pivot = end-1;
    int* pivotValue = &array[pivot];

    //printf("pivot val:%d\n",*pivotValue);

    int belowPivot = start -1;
    for (long i = start; i < start + length; ++i)
    {
        int* currentValue = &array[i];
        
        if(*currentValue < *pivotValue){
          //printf("currentval:%d less than pivot value:%d\n", *currentValue, *pivotValue);
          belowPivot++;   
          if(i>belowPivot){

            std::swap(array[belowPivot], array[i]);
          }     
          continue;
        }

        //current value more than pivot value
    }

    std::swap(array[belowPivot+1], array[pivot]);
	
    //printf("start:%d mid:%d end:%d\n",start,(belowPivot +1),end);

    //level 0 uses 1 thread
    //level 1 uses 2 threads
    //level 2 uses 4 threads
    int threadsUsed = std::pow(2,level);

    if(threadsUsed>=numThreads || level> 5){
      //has expanded into the max threads available
      //printf("utilising all cores, switching to serial -reusing threads; level=%d; length=%d\n",level,length);
      sortArrayInternal(start,belowPivot+1, level+1, (pathID + "L"));
      sortArrayInternal(belowPivot+2,end, level + 1, (pathID + "R"));
    }else{
      //can still branch into new threads
      //printf("TU:%d",threadsUsed);
      printf("%s branching from thread %d at level %d - creating two new threads. current thread terminating\n" ,pathID.c_str(), omp_get_thread_num(), level);

      #pragma omp task 
      {sortArrayInternal(start,belowPivot+1, level+1, (pathID + "L"));}

      //#pragma omp task 
      {sortArrayInternal(belowPivot+2,end, level + 1, (pathID + "R"));}
    }

    

  }

};

#endif
