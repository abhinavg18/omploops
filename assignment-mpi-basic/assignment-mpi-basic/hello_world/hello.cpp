#include <mpi.h>
#include <unistd.h>
#include <iostream>

int main(int argc, char*argv[]) {
	
	int rank;
	int size;
	
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	char mname[128] = ""; //machine name
	gethostname(mname, sizeof(mname));
	printf("I am process %d out of %d. I am running on %s\n", rank, size, mname);
	MPI_Finalize();

	return 0;
}
