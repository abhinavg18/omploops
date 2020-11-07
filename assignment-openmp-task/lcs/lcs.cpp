#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <omp.h>
#include <sys/time.h>


using namespace std;
#ifdef __cplusplus
extern "C" {
#endif

  void generateLCS(char* X, int m, char* Y, int n);
  void checkLCS(char* X, int m, char* Y, int n, int result);

#ifdef __cplusplus
}
#endif
int lcsflat( char *X, char *Y, int m, int n , int **L, int start,int end) 
{ 
   
   /* Following steps build L[m+1][n+1] in bottom up fashion. Note  
 *       that L[i][j] contains length of LCS of X[0..i-1] and Y[0..j-1] */
       
       if (start == 0 || end == 0) 
       {
        
         L[start][end] = 0; 
     
       }
   
       else if (X[start-1] == Y[end-1]) 
       {
         L[start][end] = L[start-1][end-1] + 1; 
       }
   
       else
         {
             L[start][end] = max(L[start-1][end], L[start][end-1]); 
      
        }
        
}
int lcsparallel(char* X,char *Y, int m , int n , int **L, int row)
{
      //cout<<row<<endl;
        #pragma omp parallel for schedule(guided) 
	for(int i =1;i<=row;i++)
	{
		if(i!=0&&row-i!=0&&i<=m&&row-i<=n)
		{
		        //#pragma omp task	
			//cout<<i<<" "<<row-i<<endl;
			lcsflat(X,Y,m,n,L,i,row - i);
			//cout<<L[i][row-i]<<endl;
		}
	}

        #pragma omp taskwait
	
	//lcscall(X,Y,m,n,L,1,row);



        //#pragma omp taskwait
	
	if(row > m+n)
	{
		//cout<<"return value";
		cout<<L[m][n]<<endl;
		return L[m][n];
	}

	lcsparallel(X,Y,m,n,L,row+1);

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
      cerr<<"something is amiss"<<endl;
    }
  }

  if (argc < 4) { cerr<<"usage: "<<argv[0]<<" <m> <n> <nbthreads>"<<endl;
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
   /*  #pragma omp parallel
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
		for(int j = 1; j<=maxim;j++)
		{
			if (X[k-1] == Y[j-1]){
					C[k][j] = C[k-1][j-1] + 1;
			}else{
				C[k][j] = (C[k][j-1]>C[k-1][j])? C[k][j-1] : C[k-1][j];
			}
		}
	}
	  /*  #pragma omp task shared(X, Y, C, k, maxim) 
	{ 
		#pragma omp parallel for schedule(guided, granularity)
		for(int i = k;i<=maxim;i++)
		{
			if (X[i-1] == Y[k-1]){
					C[i][k] = C[i-1][k-1] + 1;
			}else{
				C[i][k] = (C[i][k-1] > C[i-1][k])? C[i][k-1] : C[i-1][k];
			}
		}
	} */
		#pragma omp taskwait

/*	}

	result = C[m][n];
	}
	} */
	int **L = new int*[m+1];
   for(int i = 0;i<=m;i++)
   {
      L[i] = new int[n+1];
      L[i][0] = 0;
   }
   for(int i = 0;i<=n;i++)
	   L[0][i] = 0;
	  int row = 2;
	 #pragma omp parallel
  {
    #pragma omp single
    {
    //result = lcsnew(X,Y,m,n,nbthreads,L,0,0);
    result = lcsparallel(X,Y,m,n,L,row);
    cout<<result;
    //result = lcs(X,Y,m,n,nbthreads);

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
  cerr<<total<<endl;


  return 0;
}
