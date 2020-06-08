#include <iostream>
#include <random>
#include <vector>
#include <string.h>
#include "omp.h"

#define SIZE 32

int main(int argc, char **argv) {
  
  int *arr = new int[SIZE];
  
  for (int i = 0; i < SIZE; i++) {
    arr[i] = 0;
  }
  
  unsigned no_max_threads = atoi(argv[1]);
  
  
  for (int nt=1; nt <= no_max_threads; nt*=2){
    double cp1 = omp_get_wtime();
#pragma omp parallel num_threads(nt)
    {
      int loc = 0;
      int tid = omp_get_thread_num();
      for (int j = 0 ; j < 2<<25; j++) {
	int random_number = (tid * 1111) % 32;
	loc += random_number;
	loc = loc % 32;
	arr[loc] += 1;
      }
    }
    std::cout << nt << " Threads: " << omp_get_wtime() - cp1 << std::endl;
  }
  
  
  
  
}

