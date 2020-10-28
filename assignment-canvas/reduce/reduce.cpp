#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>


#ifdef __cplusplus
extern "C" {
#endif
    void generateReduceData (int* arr, size_t n);
#ifdef __cplusplus
}
#endif


int main (int argc, char* argv[]) {
	omp_set_num_threads(atoi(argv[2]));
    std::string staticStr ("static");
    std::string dynamicStr ("dynamic");
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
    
    if (argc < 5) {
        std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads> <scheduling> <granularity>"<<std::endl;
        return -1;
    }
    
    int n = atoi(argv[1]);
    int * arr = new int [n];
    
    generateReduceData (arr, atoi(argv[1]));
    
    // insert reduction code here

    //std::chrono::high_resolution_clock::time_point start;
	struct timeval start, end;

	
    int result = arr[0];
    if(staticStr.compare(argv[3])==0){
        omp_set_schedule(omp_sched_static, chunkSize);
    }
    else if(dynamicStr.compare(argv[3])==0){
        omp_set_schedule(omp_sched_dynamic, chunkSize);
    }
    else
    {
        omp_set_schedule(omp_sched_guided, chunkSize);
    }
    //start = std::chrono::high_resolution_clock::now();
	gettimeofday(&start, NULL);
#pragma omp parallel reduction(+:result)
    {


#pragma omp for schedule(runtime)
        for(int i=1; i < atoi(argv[1]); i++ )
            result += arr[i];
    }
  //  std::chrono::high_resolution_clock::time_point end  = std::chrono::high_resolution_clock::now();
	gettimeofday(&end, NULL);

	double last_time = (end.tv_sec – start.tv_sec + (end.tv_usec - start.tv_usec)/1000000.0); // in seconds
    //std::chrono::duration<double> elapsed_seconds = end-start;

    std::cerr<<last_time<<std::endl;


    delete[] arr;
    
    return 0;
}
