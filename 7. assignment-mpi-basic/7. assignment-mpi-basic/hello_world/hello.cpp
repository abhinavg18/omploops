#include <mpi.h>
#include <unistd.h>
#include <iostream>

int main(int argc, char*argv[]) {
	
	int rank;
	int size;
	char mname[128] = ""; //machine name
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	gethostname(mname, sizeof(mname));
	printf("I am process %d out of %d. I am running on %s\n", rank, size, myhostname);
	MPI_Finalize();

	return 0;
}
