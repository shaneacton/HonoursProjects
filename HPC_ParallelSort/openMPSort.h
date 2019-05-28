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

  void sortArrayInternal(int start,int end, std::string pathID);

  void genArray(long nElements,int expNo){
    numElements = nElements;
    array = ArrayUtils::generateArray(nElements, expNo);
  }



  void sortArray(int length){
    numThreads= omp_get_max_threads(); 
    //printf("omp sorting using %d threads\n",numThreads);
    
    #pragma omp parallel
    {
      #pragma omp single nowait
      {
        sortArrayInternal(0,length, "");
          
      }
    }
  
  }

    
    
  

  void sortArrayInternal(int start,int end, std::string pathID){
    //printf("%s, %d; curr:%d ; max:%d\n", pathID.c_str(), omp_get_thread_num(), omp_get_num_threads() , numThreads);
  	int length = end-start;
    if(length<= 1){
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
	

    #pragma omp  task
    {sortArrayInternal(start,belowPivot+1, (pathID + "L"));}

    sortArrayInternal(belowPivot+2,end, (pathID + "R"));
   

  }

};

#endif
