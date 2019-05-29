#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>
#include "mpi.h"

int i,j,k;
int N = 64;


int cmp(const void * a, const void * b) {
  if (*(int*)a < *(int*)b) return -1;
  if (*(int*)a > *(int*)b) return 1;
  else return 0;
}

void phase1(int *array, int N, int startIndex, int subArraySize, int *pivots, int p) {
  qsort(array + startIndex, subArraySize, sizeof(array[0]), cmp);

  for (i = 0; i < p; i++) {
    pivots[i] = array[startIndex + (i * (N / (p * p)))];    
  }
  return;
}

void phase2(int *array, int startIndex, int subArraySize, int *pivots, int *partitionSizes, int p, int myId) {
  int *collectedPivots = (int *) malloc(p * p * sizeof(pivots[0]));
  int *phase2Pivots = (int *) malloc((p - 1) * sizeof(pivots[0]));          //主元
  int index = 0;

  
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

void phase3(int *array, int startIndex, int *partitionSizes, int **newPartitions, int *newPartitionSizes, int p) {
  int totalSize = 0;
  int *sendDisp = (int *) malloc(p * sizeof(int));
  int *recvDisp = (int *) malloc(p * sizeof(int));

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

void phase4(int *partitions, int *partitionSizes, int p, int myId, int *array) {
  int *sortedSubList;
  int *recvDisp, *indexes, *partitionEnds, *subListSizes, totalListSize;

  indexes = (int *) malloc(p * sizeof(int));
  partitionEnds = (int *) malloc(p * sizeof(int));
  indexes[0] = 0;
  totalListSize = partitionSizes[0];
  for ( i = 1; i < p; i++) {
    totalListSize += partitionSizes[i];
    indexes[i] = indexes[i-1] + partitionSizes[i-1];
    partitionEnds[i-1] = indexes[i];
  }
  partitionEnds[p - 1] = totalListSize;

  sortedSubList = (int *) malloc(totalListSize * sizeof(int));
  subListSizes = (int *) malloc(p * sizeof(int));
  recvDisp = (int *) malloc(p * sizeof(int));

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

void psrs_mpi(int *array, int N)    
{
    int p, myId, *partitionSizes, *newPartitionSizes, nameLength;
    int subArraySize, startIndex, endIndex, *pivots, *newPartitions;
    char processorName[MPI_MAX_PROCESSOR_NAME];


    MPI_Comm_size(MPI_COMM_WORLD,&p);
    MPI_Comm_rank(MPI_COMM_WORLD,&myId);
    MPI_Get_processor_name(processorName,&nameLength);

    //printf("Process %d is on %s\n",myId, processorName);

    pivots = (int *) malloc(p*sizeof(int));
    partitionSizes = (int *) malloc(p*sizeof(int));
    newPartitionSizes = (int *) malloc(p*sizeof(int));
    for ( k = 0; k < p; k++) {
      partitionSizes[k] = 0;
    }

    startIndex = myId * N / p;
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

    // if (myId == 0) 
    //  for(k = 0; k < N; k++){
    //     printf("%d ",array[k]);
    //  }
    //  printf("\n");
    if (p > 1) {
      free(newPartitions);
    }
    free(partitionSizes);
    free(newPartitionSizes);
    free(pivots);


  //free(array);
  //MPI_Finalize();

}

// int main(int argc, char *argv[]) {

//   int *array;
//   array = (int *) malloc(N*sizeof(int));

//     srand(100);
//     for ( k = 0; k < N; k++) {
//       array[k] = rand()%100;
//     }
//     MPI_Init(&argc,&argv); 
//     psrs_mpi(array,N);      

//   return 0;
// }