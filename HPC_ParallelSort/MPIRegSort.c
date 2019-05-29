//https://github.com/shao-xy/mpi-psrs/blob/master/PSRS.c/

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>
#include "mpi.h"

long i,j,k;
long N = 64;


int cmp(const void * a, const void * b) {
  if (*(int*)a < *(int*)b) return -1;
  if (*(int*)a > *(int*)b) return 1;
  else return 0;
}

void phase1(int *array, long N, long startIndex, long subArraySize, int *pivots, int p) {
  qsort(array + startIndex, subArraySize, sizeof(array[0]), cmp);

  for (i = 0; i < p; i++) {
    pivots[i] = array[startIndex + (i * (N / (p * p)))];    
  }
  return;
}

void phase2(int *array, long startIndex, long subArraySize, int *pivots, long *partitionSizes, int p, int myId) {
  int *collectedPivots = (int *) malloc(p * p * sizeof(pivots[0]));
  int *phase2Pivots = (int *) malloc((p - 1) * sizeof(pivots[0])); 
  long index = 0;

  MPI_Gather(pivots, p, MPI_INT, collectedPivots, p, MPI_INT, 0, MPI_COMM_WORLD);       
  if (myId == 0) {

    qsort(collectedPivots, p * p, sizeof(pivots[0]), cmp);

    for (i = 0; i < (p -1); i++) {
      phase2Pivots[i] = collectedPivots[(((i+1) * p) + (p / 2)) - 1];
    }
  }

  MPI_Bcast(phase2Pivots, p - 1, MPI_INT, 0, MPI_COMM_WORLD);
  for ( i = 0; i < subArraySize; i++) {
    if (array[startIndex + i] > phase2Pivots[index]) {
      index += 1;
    }
    if (index == p) {
      partitionSizes[p - 1] = subArraySize - i + 1;
      break;
    }
    partitionSizes[index]++ ;   
  }
  free(collectedPivots);
  free(phase2Pivots);
  return;
}

void phase3(int *array, long startIndex, long *partitionSizes, int **newPartitions, long *newPartitionSizes, int p) {
  long totalSize = 0;
  long *sendDisp = (long *) malloc(p * sizeof(long));
  long *recvDisp = (long *) malloc(p * sizeof(long));

  MPI_Alltoall(partitionSizes, 1, MPI_INT, newPartitionSizes, 1, MPI_INT, MPI_COMM_WORLD);

  for ( i = 0; i < p; i++) {
    totalSize += newPartitionSizes[i];
  }
  *newPartitions = (int *) malloc(totalSize * sizeof(int));

  sendDisp[0] = 0;
  recvDisp[0] = 0;    
  for ( i = 1; i < p; i++) {
    sendDisp[i] = partitionSizes[i - 1] + sendDisp[i - 1];
    recvDisp[i] = newPartitionSizes[i - 1] + recvDisp[i - 1];
  }

  MPI_Alltoallv(&(array[startIndex]), partitionSizes, sendDisp, MPI_INT, *newPartitions, newPartitionSizes, recvDisp, MPI_INT, MPI_COMM_WORLD);

  free(sendDisp);
  free(recvDisp);
  return;
}

void phase4(int *partitions, long *partitionSizes, int p, int myId, int *array) {
  int *sortedSubList;
  long *subListSizes, *indexes, totalListSize, *partitionEnds, *recvDisp;

  indexes = (long *) malloc(p * sizeof(long));
  partitionEnds = (long *) malloc(p * sizeof(long));
  indexes[0] = 0;
  totalListSize = partitionSizes[0];
  for ( i = 1; i < p; i++) {
    totalListSize += partitionSizes[i];
    indexes[i] = indexes[i-1] + partitionSizes[i-1];
    partitionEnds[i-1] = indexes[i];
  }
  partitionEnds[p - 1] = totalListSize;

  sortedSubList = (int *) malloc(totalListSize * sizeof(int));
  subListSizes = (long *) malloc(p * sizeof(long));
  recvDisp = (long *) malloc(p * sizeof(long));

  for ( i = 0; i < totalListSize; i++) {
    int lowest = INT_MAX;
    int ind = -1;
    for (j = 0; j < p; j++) {
      if ((indexes[j] < partitionEnds[j]) && (partitions[indexes[j]] < lowest)) {
    lowest = partitions[indexes[j]];
    ind = j;
      }
    }
    sortedSubList[i] = lowest;
    indexes[ind] += 1;
  }

  MPI_Gather(&totalListSize, 1, MPI_INT, subListSizes, 1, MPI_INT, 0, MPI_COMM_WORLD);

  if (myId == 0) {
    recvDisp[0] = 0;
    for ( i = 1; i < p; i++) {
      recvDisp[i] = subListSizes[i - 1] + recvDisp[i - 1];
    }
  }

  MPI_Gatherv(sortedSubList, totalListSize, MPI_INT, array, subListSizes, recvDisp, MPI_INT, 0, MPI_COMM_WORLD);

  free(partitionEnds);
  free(sortedSubList);
  free(indexes);
  free(subListSizes);
  free(recvDisp);
  return;
}

void psrs_mpi(int *array, long N)    
{
    printf("sorting in c\n");
    int p, myId, nameLength;
    int *pivots, *newPartitions;
    long startIndex, endIndex, subArraySize , *partitionSizes, *newPartitionSizes;
    char processorName[MPI_MAX_PROCESSOR_NAME];


    MPI_Comm_size(MPI_COMM_WORLD,&p);
    MPI_Comm_rank(MPI_COMM_WORLD,&myId);
    MPI_Get_processor_name(processorName,&nameLength);

    if(p==0){
      printf("WARNING!: p=0");
    }

    printf("Process %d is on %s\n",myId, processorName);

    pivots = (int *) malloc(p*sizeof(int));
    partitionSizes = (long *) malloc(p*sizeof(long));
    newPartitionSizes = (long *) malloc(p*sizeof(long));
    for ( k = 0; k < p; k++) {
      partitionSizes[k] = 0;
    }

    printf("before divP\n");
    startIndex = myId * N / p;
    printf("after divP\n");
    if (p == (myId + 1)) {
      endIndex = N;
    } 
    else {
      endIndex = (myId + 1) * N / p;
    }
    subArraySize = endIndex - startIndex;

    MPI_Barrier(MPI_COMM_WORLD);

    phase1(array, N, startIndex, subArraySize, pivots, p);
    if (p > 1) {
      phase2(array, startIndex, subArraySize, pivots, partitionSizes, p, myId);
      phase3(array, startIndex, partitionSizes, &newPartitions, newPartitionSizes, p);
      phase4(newPartitions, newPartitionSizes, p, myId, array);
    }

    if (p > 1) {
      free(newPartitions);
    }
    free(partitionSizes);
    free(newPartitionSizes);
    free(pivots);


  

}