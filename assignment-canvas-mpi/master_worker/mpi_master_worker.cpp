#include <mpi.h>
#include <iostream>
using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif
float calculate_int(int functionId, float a, int res[], int intensity,  float mid)
{
  float sum = 0;

  for (int i=res[0]; i<res[1]; i++){

    float x = (a + (i + 0.5) * mid);

    switch(functionId){
      case 1:
      sum +=f1(x, intensity);
      break;

      case 2:
      sum +=f2(x, intensity);
      break;

      case 3:
      sum +=f3(x, intensity);
      break;

      case 4:
      sum +=f4(x, intensity);
      break;
    }

  }
  return sum;
}

int main (int argc, char* argv[]) {

  if (argc < 6) {
    std::cerr<<"usage: mpirun "<<argv[0]<<" <functionid> <a> <b> <n> <intensity>"<<std::endl;
    return -1;
  }

   MPI_Init(&argc, &argv);
  double starttime, endtime;
  int functionId = stoi(argv[1]);
  float a = stof(argv[2]);
  float b = stof(argv[3]);
  int n = stoi(argv[4]);
  float intensity = stoi(argv[5]);

  float mid = ((b - a) / n );

  float partial_sum = 0, total_sum = 0;
  int rank, size;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  
  int chunk_size = n/(8*(size-1));
  MPI_Status status; 	
  starttime = MPI_Wtime();

  if(rank == 0){

    int chunk_sent = 0, pointer,arr[2];
    for(int i=1; i< size; i++){

      arr[0] = (i - 1) * chunk_size;
      arr[1] = i * chunk_size;
      pointer = arr[1];
      chunk_sent++;

      if(arr[0] >= n){
        arr[0] = -1;
        chunk_sent--;
      }else if(arr[1]>n){
        arr[1] = n;
      }

      MPI_Send(arr, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
    }
    while(chunk_sent != 0){

      MPI_Recv(&partial_sum, 1, MPI_FLOAT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
      chunk_sent--;
      total_sum+=partial_sum;
      arr[0] = pointer;
      arr[1] = pointer + chunk_size;
      pointer = arr[1];
      chunk_sent++;

      if(arr[0] >= n){
        arr[0] = -1;
        chunk_sent--;
      }else if(arr[1]>n){
        arr[1] = n;
      }

      MPI_Send(arr, 2, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
    }
  }
  else{
      int worker_range[2];

      float sum;

      MPI_Recv(worker_range, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

      while(worker_range[0]!=-1){
        sum = calculate_int(functionId, a, worker_range, intensity, mid);
        MPI_Send(&sum, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
        MPI_Recv(worker_range, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      }
    }
	endtime = MPI_Wtime();
      
    if(rank == 0){
      cout<<total_sum*mid;
    }
	double totaltime= endtime - starttime;
  

	  if(rank == 0)
	  {
		 cerr<<totaltime;
	  }
    MPI_Finalize();

  return 0;
}
