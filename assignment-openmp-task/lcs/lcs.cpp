#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <omp.h>
#include<chrono>

using namespace std;

int **L;

#ifdef __cplusplus
extern "C" {
#endif

  void generateLCS(char* X, int m, char* Y, int n);
  void checkLCS(char* X, int m, char* Y, int n, int result);

#ifdef __cplusplus
}
#endif


int lcs( char *X, char *Y, int m, int n , int nbthreads) 
{ 
   int limit = 50;
   int **L = new int*[m+1];
   for(int i = 0;i<=m;i++)
      L[i] = new int[n+1];
   
   int i, j; 
   
   /* Following steps build L[m+1][n+1] in bottom up fashion. Note  
 *       that L[i][j] contains length of LCS of X[0..i-1] and Y[0..j-1] */
   #pragma omp parallel for schedule(guided,limit)
   for (i=0; i<=m; i++) 
   { 
     for (j=0; j<=n; j++) 
     { 
       if (i == 0 || j == 0) 
       {
        
         L[i][j] = 0; 
     
       }
   
       else if (X[i-1] == Y[j-1]) 
       {
         L[i][j] = L[i-1][j-1] + 1; 
       }
   
       else
         {
             L[i][j] = max(L[i-1][j], L[i][j-1]); 
      
        }
        //#pragma omp taskwait

     
   /* L[m][n] contains length of LCS for X[0..n-1] and Y[0..m-1] */
	} 
      }

   return L[m][n]; 
}
int lcsnew( char *X, char *Y, int m, int n , int nbthreads, int **L, int start,int end) 
{ 
   int limit = 50;
   
   //int i, j; 
   
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
        
        if(start == m && end==n)
            return L[m][n];
        
        if(end == n && start<m)
            lcsnew(X,Y,m,n,nbthreads,L,start+1,0);
        
        else
            lcsnew(X,Y,m,n,nbthreads,L,start,end+1);
        
        //#pragma omp taskwait

     
   /* L[m][n] contains length of LCS for X[0..n-1] and Y[0..m-1] */

   //return L[m][n]; 
}
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
void lcscall(char* X, char *Y, int m , int n, int **L, int i,int row)
{
       
        //cout<<i<<" "<<row<<endl;	
	if(i>row)
		return;
	if(i!=0&&row-i!=0&&i<=m&&row-i<=n)
	{
	  //cout<<i<<" "<<row-i<<endl;
	  lcsflat(X,Y,m,n,L,i,row-i);
	}
	
        //#pragma omp task 
	lcscall(X,Y,m,n,L,i+1,row);
        //#pragma omp taskwait


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
      std::cerr<<"something is amiss"<<std::endl;
    }
  }

  if (argc < 4) { std::cerr<<"usage: "<<argv[0]<<" <m> <n> <nbthreads>"<<std::endl;
    return -1;
  }

  int m = atoi(argv[1]);
  int n = atoi(argv[2]);
  int nbthreads = atoi(argv[3]);

  // get string data 
  char *X = new char[m];
  char *Y = new char[n];
  generateLCS(X, m, Y, n);

  
  //insert LCS code here.
  int result = -1; // length of common subsequence


   int **L = new int*[m+1];
   for(int i = 0;i<=m;i++)
   {
      L[i] = new int[n+1];
      L[i][0] = 0;
   }
   for(int i = 0;i<=n;i++)
	   L[0][i] = 0;


  
 std::chrono::time_point<std::chrono::system_clock> start_clock, end_clock;
   start_clock = std::chrono::system_clock::now();
  //result = lcs(X,Y,m,n,nbthreads);
  omp_set_num_threads(nbthreads);
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
  end_clock = std::chrono::system_clock::now();
std::chrono::duration<double> elapsed_seconds = end_clock-start_clock;
  cerr<<elapsed_seconds.count();

  checkLCS(X, m, Y, n, result);


  return 0;
}