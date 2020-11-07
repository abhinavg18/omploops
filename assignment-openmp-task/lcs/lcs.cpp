#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <omp.h>
#include <sys/time.h>



#ifdef __cplusplus
extern "C" {
#endif

  void generateLCS(char* X, int m, char* Y, int n);
  void checkLCS(char* X, int m, char* Y, int n, int result);

#ifdef __cplusplus
}
#endif



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

  if (argc < 4) { std::cerr<<"usage: "<<argv[0]<<" <m> <n> <nbthreads>"<<std::endl;
    return -1;
  }

  int m = atoi(argv[1]);
  int n = atoi(argv[2]);

  // get string data 
  char *X = new char[m];
  char *Y = new char[n];
  generateLCS(X, m, Y, n);
  int nbthreads = atoi(argv[3]);
  omp_set_num_threads(nbthreads);
  int maxim = (n>m)? n : m;

  
  //insert LCS code here.
  int result = -1; // length of common subsequence
  int C[maxim+1][maxim+1];
    #pragma omp parallel
	{
	#pragma omp for
	for(int i=0;i<=maxim;i++){
	C[0][i]=0;
	}

	#pragma omp for
	for(int i=0;i<=maxim;i++){
	C[i][0]=0;
	}
	}
	 struct timeval start, end;
	gettimeofday(&start, NULL);
	
	#pragma omp parallel
	{
	#pragma omp single
	{
	for(int k=1; k<=maxim;k++)
	{


		int granularity = 250;
		if(maxim<=10)
			granularity = 10;
	       else if (maxim <=100)
			granularity = 50;
		else 
			granularity = 5*maxim*0.01;


		if (X[k-1] == Y[k-1]){
				C[k][k] = C[k-1][k-1] + 1;
		}else{
			C[k][k] = (C[k][k-1]>C[k-1][k])? C[k][k-1] : C[k-1][k];
		}
		#pragma omp task shared(X , Y, C, k, maxim)
	{
		#pragma omp parallel for schedule(guided, granularity)
		for(int j = k; j<=maxim;j++)
		{
			if (X[k-1] == Y[j]){
					C[k][j] = C[k-1][j-1] + 1;
			}else{
				C[k][j] = (C[k][j-1]>C[k-1][j])? C[k][j-1] : C[k-1][j];
			}
		}
	}
	   #pragma omp task shared(X, Y, C, k, maxim) 
	{ 
		#pragma omp parallel for schedule(guided, granularity)
		for(int i = k;i<=maxim;i++)
		{
			if (X[i] == Y[k-1]){
					C[i][k] = C[i-1][k-1] + 1;
			}else{
				C[i][k] = (C[i][k-1] > C[i-1][k])? C[i][k-1] : C[i-1][k];
			}
		}
	}
		#pragma omp taskwait

	}

	result = C[m][n];
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
  checkLCS(X, m, Y, n, result);
  std::cerr<<total<<std::endl;


  return 0;
}
