/**
 sequentialSort.h file:
 Shane Acton
 ACTSHA001
 */
#ifndef SEQSORT_H
#define SEQSORT_H

#include "arrayUtils.cpp"

#include <string>
#include <sstream>


namespace SequentialSort {

  long numElements;
  int* array;

  void genArray(long nElements,int expNo){
    numElements = nElements;
    //printf("generating array of length %ld\n", numElements);
    array = ArrayUtils::generateArray(nElements, expNo);
    //ArrayUtils::printArray(array, numElements);
  
    //printf("sorted:%d\n" , ArrayUtils::isSorted(array, numElements));
  }

  void sortArray(int start,int end){
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

    sortArray(start,belowPivot+1);
    sortArray(belowPivot+2,end);
  }
  
};



#endif