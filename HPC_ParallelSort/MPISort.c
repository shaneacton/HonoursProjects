//http://monismith.info/cs599/examples.html

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>


void quicksort(int *, long, long);
int partition(int *, long, long);
int choosePivot(int *, long, long);
void swap(int *, int *);

void mpiSort(int *arr, long N){

  int rank, size, pivot, recvSize;
  int partner;
  MPI_Status status;

  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);


  int * newArr;
  int * recvBuffer = (int *) malloc(sizeof(int)*N/size);

  long i, j;

  if(rank == 0)
  {
    pivot = arr[choosePivot(arr, 0, N/size-1)];
    //printf("The pivot is %d\n", pivot);
  }

  MPI_Bcast(&pivot, 1, MPI_INT, 0, MPI_COMM_WORLD);

  //Assume that the number of processes is a power of 2

  int storeIdx = 0;
  long arrSize = N/size;

  //printf("1, size=%d, size/2=%d\n",size, (size/2));


  for(partner = size/2; partner > 0; partner = partner >> 1)
  {
    //printf("2\n");
    storeIdx = 0;
    for(i = 0; i < arrSize; i++)
    {
      if(arr[i] < pivot)
      {
        swap(&arr[i], &arr[storeIdx]);
        storeIdx++;
      }
    }
    //printf("storeIdx = %d in process %d\n", storeIdx, rank);

    int flag = 0;
    MPI_Request request, requestSend;
    if( (rank / partner) % 2 == 0)
    {

      //printf("rank + partner is %d\n", rank + partner);

      int sendVal = arrSize-storeIdx;
      recvSize = 0;
      MPI_Isend(&sendVal, 1, MPI_INT, rank+partner, partner+size, MPI_COMM_WORLD, &requestSend);
      MPI_Irecv(&recvSize, 1, MPI_INT, rank+partner, partner+size, MPI_COMM_WORLD, &request);
      MPI_Wait(&request, &status);


      if(arrSize-storeIdx > 0)
      {
        MPI_Isend(arr+storeIdx, arrSize - storeIdx, MPI_INT, rank + partner, partner,MPI_COMM_WORLD, &requestSend);
      }

      //printf("recvsize is %d in process %d\n", recvSize, rank);

      if(recvSize > 0)
      {
        free((void *) recvBuffer);
        recvBuffer = (int *) malloc(sizeof(int)*recvSize);

        MPI_Irecv(recvBuffer, recvSize, MPI_INT, rank + partner, partner,
                         MPI_COMM_WORLD, &request);

        MPI_Wait(&request, &status);
      }
    }
    else
    {
      int sendVal = storeIdx;
      recvSize = 0;
      MPI_Isend(&sendVal, 1, MPI_INT, rank-partner, partner+size, MPI_COMM_WORLD, &requestSend);
      MPI_Irecv(&recvSize, 1, MPI_INT, rank-partner, partner+size, MPI_COMM_WORLD, &request);
      MPI_Wait(&request, &status);
      //printf("rank - size is %d\n", rank - partner);

      if(storeIdx > 0)
      {
        MPI_Isend(arr, storeIdx, MPI_INT, rank - partner, partner, MPI_COMM_WORLD, &requestSend);
      }

      //printf("recvsize is %d in process %d\n", recvSize, rank);
      if(recvSize > 0)
      {
        free((void *) recvBuffer);
        recvBuffer = (int *) malloc(sizeof(int)*recvSize);

        MPI_Irecv(recvBuffer, recvSize, MPI_INT, rank - partner, partner,
                         MPI_COMM_WORLD, &request);

        MPI_Wait(&request, &status);
      }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    if(recvSize > 0)
    {
      //Merge arrays
      if((rank / partner) % 2 == 0) //Keep smaller elements
      {
        newArr = (int *) malloc(sizeof(int)*(recvSize+storeIdx));
        for(i = 0; i < storeIdx; i++)
          newArr[i] = arr[i];
        for(j = 0, i = storeIdx; i < recvSize+storeIdx; i++, j++)
          newArr[i] = recvBuffer[j];
        free((void *) arr);
        arr = newArr;
        newArr = NULL;
        arrSize = recvSize+storeIdx;
      }
      else  //Keep larger elements
      {
        newArr = (int *) malloc(sizeof(int)*(recvSize+(arrSize-storeIdx)));
        for(j = 0, i = storeIdx; i < arrSize; i++, j++)
          newArr[j] = arr[i];
        for(j = 0, i = arrSize-storeIdx; i < recvSize+(arrSize-storeIdx); i++, j++)
          newArr[i] = recvBuffer[j];
        free((void *) arr);
        arr = newArr;
        newArr = NULL;
        arrSize = recvSize+(arrSize-storeIdx);
      }
    }
    else
    {
      arrSize = 0;
    }

    if(rank % partner == 0)
    {
      pivot = arr[choosePivot(arr, 0, arrSize-1)];
    
      for(i = 1; i < partner; i++)
        MPI_Send(&pivot, 1, MPI_INT, rank+i, partner+1, MPI_COMM_WORLD);
    }
    else
    {
      MPI_Recv(&pivot, 1, MPI_INT, partner*(rank/partner), partner+1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

  }

  //printf("3\n");

  if(arrSize > 0)
    quicksort(arr, 0, arrSize-1);

  //printf("4\n");

  int * fullArr, * displacement;
  long  * sizeArr;
  //printf("4.1\n");
  if(rank == 0)
  {
    sizeArr = (long *) malloc(sizeof(long)*size);
    //printf("4.2\n");
    fullArr = (long *) malloc(sizeof(long)*N);
    //fullArr = (int *) malloc(sizeof(long)*N);
    //("4.3\n");
    displacement = (int *) malloc(sizeof(int)*size);
    //printf("4.4\n");
  }

  //printf("5\n");

  MPI_Gather(&arrSize, 1, MPI_INT, sizeArr, 1, MPI_INT, 0, MPI_COMM_WORLD);

  if(rank == 0)
  {
    i = 0;
    displacement[0] = 0;
    //printf("displacement[%ld] = %d\n", i, displacement[i]);
    //Perform a scan on sizeArr to determine the displacement of each data location.
    for(i = 1; i < size; i++)
    {
      displacement[i] = sizeArr[i-1] + displacement[i-1];
      //printf("displacement[%ld] = %d\n", i, displacement[i]);
    }
  }

  //printf("6\n");

  MPI_Gatherv(arr, arrSize, MPI_INT, fullArr, sizeArr, displacement, MPI_INT, 0, MPI_COMM_WORLD);

  MPI_Barrier(MPI_COMM_WORLD);
  if(arrSize > 0){
    //free((void *) arr);
  }

  //printf("7\n");

  if(rank == 0)
  {

    free((void *) fullArr);
    free((void *) sizeArr);
    free((void *) displacement);
  }

}

void quicksort(int * arr, long lo, long hi)
{
  //printf("3.1\n");
  if(lo < hi)
  {
    //printf("3.2\n");
    int p = partition(arr, lo, hi);
    //printf("3.3\n");
    quicksort(arr, lo, p - 1);
    //printf("3.4\n");
    quicksort(arr, p + 1, hi);
    //printf("3.5\n");
  }
}

int partition(int * arr, long lo, long hi)
{

  //printf("3.6\n");
  long i;
  long pivotIdx = choosePivot(arr, lo, hi);
  //printf("3.65\n");
  int pivotVal = arr[pivotIdx];

  //printf("3.7\n");

  swap(&arr[pivotIdx], &arr[hi]);

  int storeIdx = lo;

  for(i = lo; i < hi; i++)
  {
    if(arr[i] < pivotVal)
    {
      swap(&arr[i], &arr[storeIdx]);
      storeIdx++;
    }
  }

  swap(&arr[storeIdx], &arr[hi]);
  return storeIdx;
}

void swap(int * x, int * y)
{
  int temp = *x;
  *x = *y;
  *y = temp;
}

//Select the median of arr[lo], arr[hi], and arr[(lo+hi)/2]
int choosePivot(int * arr, long lo, long hi)
{
  long mid = (lo+hi)/2;

  long temp;
  if(arr[lo] > arr[hi])
  {
    temp = lo;
    lo = hi;
    hi = temp;
  }
  if(arr[mid] < arr[lo])
  {
    temp = mid;
    mid = lo;
    lo = temp;
  }
  if(arr[hi] < arr[mid])
  {
    temp = mid;
    mid = hi;
    hi = temp;
  }
  return mid;
}
