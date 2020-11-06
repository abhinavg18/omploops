#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include <omp.h>

#ifdef __cplusplus
extern "C" {
#endif

  void generateMergeSortData (int* arr, size_t n);
  void checkMergeSortResult (int* arr, size_t n);

#ifdef __cplusplus
}
#endif


void merge(int * arr, int start, int mid, int end, int * temp) {



	if (start == end) return;

	if (end-start == 1) {
		if (arr[start] > arr[end]) {
			int temp = arr[start];
			arr[start] = arr[end];
			arr[end] = temp;
		}
		return;
	}

	int i, j, k;

	int n = mid - start;

	
	for (i=0; i<n; ++i){
	    temp[start+i] = arr[start+i];
        }

	i = 0;    
	j = mid;  
	k = start;    

	// merge
	while (i<n && j<=end) {
		if (temp[start+i] <= arr[j] ) {
			arr[k++] = temp[start+i];
			i++;
		} else {
			arr[k++] = arr[j++];
		}
	}


	while (i<n) {
		arr[k++] = temp[start+i];
		i++;
	}

}


void mergesort(int * arr, int start, int end, int n, int * temp, long g) {
	
	

	if (end-start > g) {
		int mid = (start+end)/2;
		
		#pragma omp task
		mergesort(arr, start, mid, n, temp, g);
		
		#pragma omp task
		mergesort(arr, mid+1, end, n, temp, g);
		
		#pragma omp taskwait
		merge(arr, start, mid+1, end, temp);
	}
	else
	{
		if (start < end) {
		    int mid = (start+end)/2;
		    mergesort(arr, start, mid, n, temp, g);
		    mergesort(arr, mid+1, end, n, temp, g);
		    merge(arr, start, mid+1, end,temp);
		}
	}
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
  
  if (argc < 3) { std::cerr<<"usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }

  int n = atoi(argv[1]);
  int nbthreads = atoi(argv[2]);
  omp_set_num_threads(nbthreads);
  // get arr data
  int * arr = new int [n];
  generateMergeSortData (arr, n);
  int * temp = new int [n];
  long g = 250;
	
	if(n<=500)    
		g = 500;	
	else if(n>500 && n < 100000){
		g = 5*n*0.02;
            }
	else if(n==100000){
		g = 5*n*0.01;
            }
	else if( n==1000000){
		g = 5*n*0.001;
            }
	else if( n==10000000){
		g = 5*n*0.0001;
            }
	else 
		g = 5000;
  //insert sorting code here.
  struct timeval start, end;
	gettimeofday(&start, NULL);
	#pragma omp parallel
	{
		#pragma omp single
		{
			mergesort(arr, 0, n-1, n, temp,g ); //mergesort called here
		}
	}
	gettimeofday(&end, NULL);
	double ssec=start.tv_sec;
  double esec=end.tv_sec;
 double secdiff= esec-ssec;
double su=start.tv_usec;
        double eu=end.tv_usec;
	double udiff= (eu-su)/1000000;
	double total= secdiff+ udiff;
	  
  
  checkMergeSortResult (arr, n);
  std::cerr<<total<<std::endl;
  
  delete[] arr;
  delete[] temp;

  return 0;
}
