#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>


#ifdef __cplusplus
extern "C" {
#endif

  void generateReduceData (int* arr, size_t n);

#ifdef __cplusplus
}
#endif

int arrSum(int* arr, int start, int end)
{
    int arrsum = 0;
    
#pragma omp parallel for reduction(+:arrsum)
    for(int i=start;i<end;i++)
    {
        arrsum += arr[i];
    }
    return arrsum;
}


int main (int argc, char* argv[]) {
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
  
  if (argc < 3) {
    std::cerr<<"usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }

  int n = atoi(argv[1]);
  int * arr = new int [n];
  int result = 0;
  int nbthreads = atoi(argv[2]);
  omp_set_num_threads(nbthreads);
  int granularity = n/(2*nbthreads);
	struct timeval start, end;

  generateReduceData (arr, atoi(argv[1]));

  //insert reduction code here
  gettimeofday(&start, NULL);
  #pragma omp parallel for

for(int i=0; i<n; i+=granularity){
  int j;
  int start = i;
  static int partsum;
  int end = start+granularity;
  if(end>n){
    end = n;
  }
  #pragma omp task
  {

     partsum = arrSum(arr, start, end);
     #pragma omp critical
      result+=partsum;
  }
 }
  	gettimeofday(&end, NULL);

  double ssec=start.tv_sec;
  double esec=end.tv_sec;
 double secdiff= et-st;
double su=start.tv_usec;
        double eu=end.tv_usec;
	double udiff= (eu-su)/1000000;
	double total= secdiff+ udiff;
	
  std::cout<<result<<std::endl;
  std::cerr<<total<<std::endl;
  
  delete[] arr;
  

  return 0;
}
