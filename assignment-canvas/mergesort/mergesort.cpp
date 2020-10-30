#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <sys/time.h>

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
   mergeSort(arr, 0, n-1, nbthreads);
  gettimeofday(&end, NULL);
  checkMergeSortResult (arr, n);
  
  
  
  double st=start.tv_sec;
        double et=end.tv_sec;
	double stet= et-st;

        double su=start.tv_usec;
        double eu=end.tv_usec;
	double tu= (eu-su)/1000000;


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

    for(int k = 1; k < n+1;k *= 2)
    {
        #pragma omp parallel for schedule(static, 1)
        for(int i=0;i < n+1;i += (2*k))
        {
            int left = i;
            int mid = i + (k-1);
            int right = i + ((2*k)-1);
            if(mid >= n)
            {#include <iostream>
#include <chrono>
#include <ratio>
#include <ctime>

#include <omp.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>


#ifdef __cplusplus
extern "C" {
#endif

    void generatePrefixSumData (int* arr, size_t n);
    void checkPrefixSumResult (int* arr, size_t n);


#ifdef __cplusplus
}
#endif


int main (int argc, char* argv[]) {

    if (argc < 3) {
        std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
        return -1;
    }
    omp_set_dynamic(0);
    omp_set_num_threads(atoi(argv[2]));
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

    int i, n = atoi(argv[1]);
    int * arr = new int [n];
    int * pr = new int [n+1];
    pr[0]=0;
    int nbthreads = atoi(argv[2]);

    int *temp ;


#pragma omp_set_schedule(omp_sched_static, chunkSize);

    generatePrefixSumData (arr, atoi(argv[1]));

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
#pragma omp parallel
    {
        int id = omp_get_thread_num();
        int sum = 0;

#pragma omp single
        {
            nbthreads = omp_get_num_threads();
            temp = new int[nbthreads];
        }
        
#pragma omp for schedule(static)
        for ( i = 0; i<n; i++ )
        {
            sum += arr[i];
            pr[i+1] = sum;
        }
        
#pragma omp critical
        temp[id] = sum;

#pragma omp barrier
        int x = 0;
        for (i=0; i<id; i++)
        {
            x += temp[i];
        }

#pragma omp for schedule(static)
        for (i=0;i< n;i++ )
        {
            pr[i+1] += x;
        }
    }
    std::chrono::high_resolution_clock::time_point end  = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed_seconds = end-start;

    checkPrefixSumResult(pr, atoi(argv[1]));

    std::cerr<<std::fixed<<elapsed_seconds.count()<<std::endl;

    
    //free memory
    delete[] arr;
    delete[] pr;
    delete[] temp;

    return 0;
}

                mid = (i+n-1)/2;
                right = n-1;
            }
            else if(right >= n)
            {
                right = n-1;
            }
            merge(arr,left,mid,right);
        }
    }
}
