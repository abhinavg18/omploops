#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string>

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



int main (int argc, char* argv[]) {
  
  if (argc < 9) {
    std::cerr<<"Usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <scheduling> <granularity>"<<std::endl;
    return -1;
  }
  
  int functionid = stoi(argv[1]);
  float a = stof(argv[2]);
  float b = stof(argv[3]);
  int n = stoi(argv[4]);
  int intensity = stoi(argv[5]);
  float mid = ((b - a) / n );
  omp_set_num_threads(stoi(argv[6]));
  string scheduling = argv[7];
  int  granularity = stoi(argv[8]);
  float result = 0.0;
  
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
  
    if("static" == scheduling){
    omp_set_schedule(omp_sched_static, granularity);
  } else if("dynamic" == scheduling){
    omp_set_schedule(omp_sched_dynamic, granularity);
  }else if("guided" == scheduling){
    omp_set_schedule(omp_sched_guided, granularity);
  }

  struct timeval start, end;

  //insert code here
gettimeofday(&start, NULL);
 #pragma omp parallel  reduction (+ : result) 
 {
	#pragma omp for schedule (runtime)
		for (int i=0; i<n; i++){
			float x = (a + (i + 0.5) * mid);

			 switch(functionid){
			   case 1:
				 result +=(f1(x, intensity));
				 break;

			   case 2:
				 result +=(f2(x, intensity));
				 break;

			   case 3:
				 result +=(f3(x, intensity));
				 break;

			   case 4:
				 result +=(f4(x, intensity));
				 break;
     }
   }
 }
gettimeofday(&end, NULL);
double st=start.tv_sec;
        double et=end.tv_sec;
	double stet= et-st;
//	std::cerr<<stet<<std::endl;

        double su=start.tv_usec;
        double eu=end.tv_usec;
	double tu= (eu-su)/1000000;
//	std::cerr<<tu<<std::endl;  
//	std::cerr<<stet + tu<<std::endl;

	double tt= stet+ tu;
	std::cout<< (result*mid) <<std::endl;
	std::cerr<<tt<<std::endl;

  
 
  return 0;
}
