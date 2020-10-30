#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <omp.h>
#include <sys/time.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif
  void generateMergeSortData (int* arr, size_t n);
  void checkMergeSortResult (int* arr, size_t n);
#ifdef __cplusplus
}
#endif
using namespace std;
void mergeSort(int* arr, int l, int r, int nbthreads);
void merge(int* arr, int l, int m, int r);

int min(int x, int y) { return (x<y)? x :y; }

int main (int argc, char* argv[]) {

if (argc < 3) { std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }
  //forces openmp to create the threads beforehand
#pragma omp parallel
  {
    int fd = open (argv[0], O_RDONLY);
    if (fd != -1) {
      close (fd);
    }
    else {
      std::cerr<<"something is amiss"<<std::endl;
    }
  }
  
  struct timeval start, end;

  int n = atoi(argv[1]);
  
  // get arr data
  int * arr = new int [n];
  generateMergeSortData (arr, n);
  int nbthreads = atoi(argv[2]);

  //insert sorting code here.
	gettimeofday(&start, NULL);
   mergeSort(arr, 0, n, nbthreads);
   gettimeofday(&end,NULL); 
  checkMergeSortResult (arr, n);
//  gettimeofday(&end, NULL);
  
  
  double st=start.tv_sec;
        double et=end.tv_sec;
	double stet= et-st;
//	std::cerr<<stet<<std::endl;

        double su=start.tv_usec;
        double eu=end.tv_usec;
	double tu= (eu-su)/1000000;
//	std::cerr<<tu<<std::endl;  
//	std::cerr<<stet + tu<<std::endl;

	double tt= stet+ tu;
	std::cerr<<tt<<std::endl;
  delete[] arr;

  return 0;
}
void merge(int* arr, int l, int m, int r)
{
    int i, j, k;
    int left = m - l + 1;

    int right =  r - m;

    int* arrLeft = new int[left];

    int* arrRight = new int[right];

    for (i = 0; i < left; i++)
        arrLeft[i] = arr[l + i];

    for (j = 0; j < right; j++)
        arrRight[j] = arr[m + 1+ j];

    i = 0;
    j = 0;

    k = l;

 
    while (i < left && j < right)
    {
        if (arrLeft[i] <= arrRight[j])
        {
            arr[k] = arrLeft[i];
            i++;
        }
        else
        {
            arr[k] = arrRight[j];
            j++;
        }
        k++;
    }

    while (i < left)
    {
        arr[k] = arrLeft[i];
        i++;
        k++;
    }

    while (j < right)
    {
        arr[k] = arrRight[j];
        j++;
        k++;
    }

   
    delete[] arrLeft;
    delete[] arrRight;
}

void mergeSort(int* arr, int l, int r, int nbthreads)
{
    omp_set_num_threads(nbthreads);

    int n=r;

    for(int k = 1; k <= n-1;k *= 2)
    {
        #pragma omp parallel for schedule(static, 1)
        for(int i=0;i < n-1;i += (2*k))
        {
			int mid = min(i + k - 1, n-1);
         
                int right_end = min(i 
                             + 2*k - 1, n-1);
         
            merge(arr,i,mid,right_end);
        }
    }
}
