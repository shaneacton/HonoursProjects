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

  // private members
  int numElements;
  int* array;

  void genArray(int nElements){
    numElements = nElements;
    printf("generating array of length %d\n", numElements);
    array = ArrayUtils::generateArray(nElements);
    ArrayUtils::printArray(array, numElements);
  }

  void sortArray(int start,int end){
    int length = end-start;
    if(length<= 1){
      return;
    }

    int pivot = end-1;
    int* pivotValue = &array[pivot];
    //*(array + pivot)

    int belowPivot = -1;
    for (int i = 0; i < length; ++i)
    {
        int* currentValue = &array[i];
        
        if(*currentValue < *pivotValue){
          belowPivot++;   
          if(i>belowPivot){
            std::swap(array[belowPivot + 1], array[i]);
          }     
          continue;
        }

        //current value more than pivot value


    }

    sortArray(start,pivot);
    sortArray(pivot,end);
  }

  void printArray(){
    ArrayUtils::printArray(array, numElements);
  }
  
};



#endif