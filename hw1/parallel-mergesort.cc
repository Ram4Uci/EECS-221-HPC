/**
 *  \file parallel-mergesort.cc
 *
 *  \brief Implement your parallel mergesort in this file.
 */

#include <assert.h>
#include <stdio.h>
#include<string.h>
#include <stdlib.h>
#include<omp.h>
#include "sort.hh"
#define keytype_size sizeof(keytype) // Size of the type keytype

int level=64; //Default value
int size;// No . of elements

void mergesort(keytype *a,keytype *b,int left,int right);
void parallelmerge(keytype *a,keytype *b,int l1,int r1,int l2,int r2,int l3);
void serialmerge(keytype *a,keytype *b,int l1,int r1,int l2,int r2,int l3);
int binarysearch(keytype *a, keytype key, int left, int right);
int compare (const void* a, const void* b);

void parallelSort (int N, keytype* A)
{
  /* Lucky you, you get to start from scratch */
  //Initializes the temporary array b and calls the mergesort routine
  keytype *b;
  b = new keytype[N]; // create a temp array to transfer merged elements .
  
  size = N;
 #pragma omp parallel 
 {
     #pragma omp master // Get the number of threads spawned. Set the level = this number.
    {
      int num = omp_get_num_threads();
      level = num;
      printf(" Number of threads = %d \n Level = %d\n",num,level);
    }

    #pragma omp single nowait // Run this on a single thread.
    {
      mergesort(A,b,0,N-1);
    }
  }
}

int binarysearch(keytype *a, keytype key, int left, int right)
{
  /* Searches of the point in the second array that splits the array such that
     all elements before the point is less than the key */
  int l = left;
  int r = right;
  int m;
  while(l<r)
    {
      m = (l+r)/2;

      if(key <=a[m])r=m;
      else l=m+1;
    }

  return r-1;
}

void serialmerge(keytype*a , keytype *b,int l1,int r1,int l2,int r2,int l3)
{
  /* Performs a serial merge */
  int i=l1;
  int j = l2;
  int k=l3;

  while(i<=r1 && j<=r2)
    {
      if(a[i]<=a[j])
	{
	  b[k++]=a[i++];

	}
      else
	{
	  b[k++] = a[j++];

	}
    }

  while(i<=r1)
    {
      b[k++]=a[i++];
    }
  while(j<=r2)
    {
      b[k++]=a[j++];
    }

}

void mergesort(keytype *a,keytype *b,int left,int right)
{ 
  /* Performs the mergesort and recursively calls itself 
     until reaching the threshold then performing quick sort */

  int num = right - left +1;
  if(num <=(size/level))
    {
      qsort(a+left,num,keytype_size,compare);
      return;
    }
  else
    {
      int mid = left + (right - left)/2;

      #pragma omp task
      {
	mergesort(a,b,left,mid);
      }

      mergesort(a,b,mid+1,right);
      
      #pragma omp taskwait

      parallelmerge(a,b,left,mid,mid+1,right,left);

      memcpy(a+left,b+left,(right-left+1)*keytype_size);
    }
}

void parallelmerge(keytype *a,keytype *b,int l1,int r1,int l2,int r2,int l3)
{
  /* Implements the parallel merge by splitting the two arrays that needed to be sorted recursively
     till the threshold is reached and then performing sequentail merge*/

  int s = r1-l1+1 + r2-l2+1;
 
  if(s<=size/level)
    {
      serialmerge(a,b,l1,r1,l2,r2,l3);
    }
  else
    {
      int m1 = (l1+r1)/2;
      int pos = binarysearch(a,a[m1],l2,r2);
      int m2 = l3 + (m1-l1+1) + (pos-l2+1);
      #pragma omp task
      {
	parallelmerge(a,b,l1,m1,l2,pos,l3);
      }
     
     parallelmerge(a,b,m1+1,r1,pos+1,r2,m2);

     #pragma omp taskwait
    }
}

int compare (const void* a, const void* b)
{
  /* Compare function for quicksort */

  keytype ka = *(const keytype *)a;
  keytype kb = *(const keytype *)b;
  if (ka < kb)
    return -1;
  else if (ka == kb)
    return 0;
  else
    return 1;
} 

/* eof */
