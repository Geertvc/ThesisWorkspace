#include <omp.h>
#include <stdio.h>
#include <vector>
#include "time.h"
#include <sys/time.h>

main ()  {
  struct timeval time1, time2;
  clock_t start, end;
  start = clock();

  gettimeofday (&time1, NULL);


  

int count = 8;
int internalCount = 1000000000;

std::vector<int> shared;
for (int m = 0; m < count; ++m)
{
  shared.push_back(0);
}

#pragma omp parallel for schedule(dynamic, 1) shared(shared)
  for (int i = 0; i < count; ++i)
  {
    for (int j = 0; j < internalCount; ++j)
    {
    }
    printf("%d is done \n", i);
    shared[i] = i;
  }


  end = clock();
  gettimeofday (&time2, NULL);
  printf("Total time used: %f seconds. \n", ((double)(end-start)/CLOCKS_PER_SEC));
  printf("Total time used: %ld usecs. \n", (time2.tv_usec-time1.tv_usec));


/*int nthreads, tid;

// Fork a team of threads with each thread having a private tid variable
#pragma omp parallel private(tid)
  {

  // Obtain and print thread id 
  tid = omp_get_thread_num();
  printf("Hello World from thread = %d\n", tid);

  for (int i = 0; i < 1000000000; ++i)
  {
    // code
  }
  printf("thread %d is finished\n", tid);

  // Only master thread does this
  if (tid == 0) 
    {
    nthreads = omp_get_num_threads();
    printf("Number of threads = %d\n", nthreads);
    }


  

  }  // All threads join master thread and terminate*/

}
