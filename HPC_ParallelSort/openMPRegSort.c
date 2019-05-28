#include "omp.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "math.h"

/* headers */
int lcompare(const void * ptr2num1, const void * ptr2num2);
int *merge(int * left, int * right, int l_end, int r_end);
int *merge_sort(int * arr, long size);
void calc_partition_borders(int array[],
               int start,
               int end,
               int sublist_sizes[],
               int at,
               int pivots[],
               int first_p,
               int last_p);
void psrs_sort(int *a, int n);
void sortll(int *a, long len);

/* sort an array in non-descending order */
void psrs_sort(int *a, int n) {
  if(n > 1){
  
  // Testing shows that our algorithm is now the quickest 
  int p, size, rsize, sample_size;
  int *sample, *pivots;
  int *partition_borders, *bucket_sizes, *result_positions;
  int **loc_a_ptrs;

  // Determine the appropriate number of threads to use
  // p^3 <= n - We need this to hold true
  p = omp_get_max_threads();
  p = p*p*p;
  if(p > n){
  p = floor(pow(n,0.33));
  p-=p%2;
  }else{
   p = omp_get_max_threads();
   p-=p%2;
  }
  omp_set_num_threads(p);

  size  = (n + p - 1) / p;
  rsize = (size + p - 1) / p;
  sample_size = p * (p - 1);

  loc_a_ptrs = malloc(p * sizeof(int *));
  sample = malloc(sample_size * sizeof(int));
  partition_borders = malloc(p * (p + 1) * sizeof(int));
  bucket_sizes = malloc(p * sizeof(int));
  result_positions = malloc(p * sizeof(int));
  pivots = malloc((p - 1) * sizeof(int));

  //printf("0\n");

  #pragma omp parallel
  {
    int i, j, max, thread_num, start, end, loc_size, offset, this_result_size;
    int *loc_a, *this_result, *current_a;

    thread_num = omp_get_thread_num();
    start = thread_num * size;
    end = start + size - 1;
    if(end >= n) end = n - 1;
    loc_size = (end - start + 1);
    end = end % size;

    //printf("0.5\n");

    loc_a = malloc(loc_size * sizeof(int));
    memcpy(loc_a, a + start, loc_size * sizeof(int));
    loc_a_ptrs[thread_num] = loc_a;

    sortll(loc_a, loc_size); // Testing shows that this sequential sort is quickest in this instance
  
    offset = thread_num * (p - 1) - 1;

    //printf("0.75\n");

    for(i = 1; i < p; i++) {
      if(i * rsize <= end) {
        sample[offset + i] = loc_a[i * rsize - 1];
      } else {
        sample[offset + i] = loc_a[end];
      }
    }

    //printf("1\n");

    #pragma omp barrier

    #pragma omp single
    {
      merge_sort(sample, sample_size); // Testing shows that this sequential sort is quickest in this instance
      for(i = 0; i < p - 1; i++) {
        pivots[i] = sample[i * p + p / 2];
      }
    }

    #pragma omp barrier

    offset = thread_num * (p + 1);
    partition_borders[offset] = 0;
    partition_borders[offset + p] = end + 1;
    calc_partition_borders(loc_a, 0, loc_size-1, partition_borders, offset, pivots, 1, p-1);

    #pragma omp barrier

    //printf("2\n");

    max = p * (p + 1);
    bucket_sizes[thread_num] = 0;
    for(i = thread_num; i < max; i += p + 1) {
      bucket_sizes[thread_num] += partition_borders[i + 1] - partition_borders[i];
    }

    #pragma omp barrier

    #pragma omp single
    {
      result_positions[0] = 0;
      for(i = 1; i < p; i++) {
        result_positions[i] = bucket_sizes[i-1] + result_positions[i-1];
      }
    }

    //printf("3\n");

    #pragma omp barrier

    this_result = a + result_positions[thread_num];

    if(thread_num == p-1) {
      this_result_size = n - result_positions[thread_num];
    } else {
      this_result_size = result_positions[thread_num+1] - result_positions[thread_num];
    }

    //printf("3.5\n");

    // pluck this threads sublist from each of the local arrays
    this_result = a + result_positions[thread_num];

    //printf("3.6\n");

    for(i = 0, j = 0; i < p; i++) {
      int low, high, partition_size;
      offset = i * (p + 1) + thread_num;
      low = partition_borders[offset];
      //printf("3.7\n");
      high = partition_borders[offset+1];
      partition_size = (high - low);

      //printf("3.8\n");
      if(partition_size > 0) {
        memcpy(this_result+j, &(loc_a_ptrs[i][low]), partition_size * sizeof(int));
        j += partition_size;
      }
    }

    //printf("4\n");

    // sort p local sorted arrays
    sortll(this_result, this_result_size); // Testing shows that this sequential sort is quickest in this instance
  
    #pragma omp barrier
    free(loc_a);
  }

  //printf ("10\n");

  free(loc_a_ptrs);
  free(sample);
  free(partition_borders);
  free(bucket_sizes);
  free(result_positions);
  free(pivots);
  }
  
}


/* determine the boundaries for the sublists of an local array */
void calc_partition_borders(int array[],    // array being sorted
                            int start,
                            int end,              // separate the array into current process range
                            int result[],
                            int at,               // this process start point in result
                            int pivots[],   // the pivot values
                            int first_pv,         // first pivot
                            int last_pv)          // last pivot
{
  int mid, lowerbound, upperbound, center;
  int pv;

  mid = (first_pv + last_pv) / 2;
  pv = pivots[mid-1];
  lowerbound = start;
  upperbound = end;
  while(lowerbound <= upperbound) {
    center = (lowerbound + upperbound) / 2;
    if(array[center] > pv) {
      upperbound = center - 1;
    } else {
      lowerbound = center + 1;
    }
  }
  result[at + mid] = lowerbound;

  if(first_pv < mid) {
    calc_partition_borders(array, start, lowerbound - 1, result, at, pivots, first_pv, mid - 1);
  }
  if(mid < last_pv) {
    calc_partition_borders(array, lowerbound, end, result, at, pivots, mid + 1, last_pv);
  }
}


/* Compare long longs (lifted from the harness) */
int lcompare(const void * ptr2num1, const void * ptr2num2)
{
  int num1 = *((int*) ptr2num1);
  int num2 = *((int*) ptr2num2);

  if ( num1 > num2 )
    return 1;
  else if ( num1 < num2 )
    return -1;
  else
    return 0;
}


/*
  Sort a portion of an array
  @todo: see if merge sort might be better in some cases
*/
void sortll(int *a, long len)
{
  qsort(a, len, sizeof(int), lcompare);
}

/*
  Standard merge sort
*/
int *merge_sort(int * arr, long size){
  // Arrays shorter than 1 are already sorted
  if(size > 1){
    long middle = size / 2, i; 
    int *left, *right;
    left = arr;
    right = arr + middle; 
    
    left = merge_sort(left, middle);
    right = merge_sort(right, size-middle);
    return merge(left, right, middle,size-middle);
  }else { return arr; }
}

int *merge(int * left, int * right, int l_end, int r_end){
  int temp_off, l_off, r_off, size = l_end+r_end;
  int *temp = malloc(sizeof(int) * l_end);

  // Copy lower half into temp buffer
  for(l_off=0, temp_off=0; left+l_off != right; l_off++, temp_off++){
    *(temp + temp_off) = *(left + l_off);
  }
  
  temp_off=0; l_off=0; r_off=0;

  while(l_off < size){
    if(temp_off < l_end){
      if(r_off < r_end){
        if(*(temp+temp_off) < *(right+r_off)){
          *(left+l_off) = *(temp+temp_off);
          temp_off++;
        }else{
          *(left+l_off) = *(right+r_off);
          r_off++;
        }
      }else{
        *(left+l_off) = *(temp+temp_off);
        temp_off++;
      }
    }else{
      if(r_off < r_end) {
        *(left + l_off) = *(right + r_off);
        r_off++;
      }else{
        printf("\nERROR - merging loop going too far\n");
      }
    }
    l_off++;
  }
  free(temp);
  return left;
}
