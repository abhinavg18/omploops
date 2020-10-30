#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>


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
  
#pragma omp_set_schedule(omp_sched_static, chunkSize);
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
  
omp_set_dynamic(0);
    omp_set_num_threads(atoi(argv[2]));
  
  int n = atoi(argv[1]);
  int nbthreads = atoi(argv[2]);
  int * arr = new int [n];
  generatePrefixSumData (arr, n);
  int i;
  int *val;
  int * pr = new int [n+1];
  pr[0]=0;
  //insert prefix sum code here
struct timeval start, end;
gettimeofday(&start, NULL);
#pragma omp parallel
    {
        int id = omp_get_thread_num();
        int sum = 0;

#pragma omp single
        {
            nbthreads = omp_get_num_threads();
            val = new int[nbthreads];
        }
        
#pragma omp for schedule(static)
        for ( i = 0; i<n; i++ )
        {
            sum += arr[i];
            pr[i+1] = sum;
        }
        
#pragma omp critical
        val[id] = sum;

#pragma omp barrier
        int x = 0;
        for (i=0; i<id; i++)
        {
            x += val[i];
        }

#pragma omp for schedule(static)
        for (i=0;i< n;i++ )
        {
            pr[i+1] += x;
        }
    }
  
  gettimeofday(&end, NULL);
  checkPrefixSumResult(pr, n);
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
