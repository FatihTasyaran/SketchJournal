#ifndef H_GENERATOR
#define H_GENERATOR

#include <iostream>
#include <omp.h>
#include <random>
#include <algorithm>
#include "common.h"

using namespace std;

void generateData(dtype* &data, unsigned long long no_stream, unsigned long long no_unique,int distNum) {
  data = new dtype[no_stream];  

  double max_rand = 0, min_rand = 0;
  double* rand_data = new double[no_stream];
if(distNum == NORMAL)
{
#pragma omp parallel num_threads(16)
  {
    int id = omp_get_thread_num();
    std::random_device rd; 
    std::mt19937 gen((rd() + id) * (id + 11111)); 
    std::normal_distribution<> ed(normal_dist_mean,normal_dist_var);    
    
    double my_max = 0;
#pragma omp for 
    for(stype i = 0; i < no_stream; i++) {
      rand_data[i] = ed(gen);
      if(rand_data[i] > my_max) {
	my_max = rand_data[i];
      }
    }

    double my_min = 0;
#pragma omp for 
    for(stype i = 0; i < no_stream; i++) {
      if(rand_data[i] < my_min) {
	my_min = rand_data[i];
      }
    }
    
#pragma omp critical 
    {
      if(my_max > max_rand) max_rand = my_max;
      if(my_min < min_rand) min_rand = my_min;
    }
  }

#pragma omp parallel for 
  for(stype i = 0; i < no_stream; i++) {
    data[i] = ((rand_data[i] - min_rand) / (max_rand - min_rand)) * (no_unique - 1);
    D(
      if(data[i] >= no_unique || data[i] < 0) {
	std::cout << "ERROR: " << rand_data[i] << " " << min_rand << " " << max_rand << " " << data[i] << std::endl;
	
      }
    );
  }
}

else if (distNum == UNIFORM)
{
  #pragma omp parallel num_threads(16)
  {
    int id = omp_get_thread_num();
    std::random_device rd; 
    std::mt19937 gen((rd() + id) * (id + 11111)); 
    std::uniform_int_distribution<> ed(uniform_param_low,uniform_param_upper);    
    
    double my_max = 0;
#pragma omp for 
    for(stype i = 0; i < no_stream; i++) {
      rand_data[i] = ed(gen);
      if(rand_data[i] > my_max) {
	my_max = rand_data[i];
      }
    }

    double my_min = 0;
#pragma omp for 
    for(stype i = 0; i < no_stream; i++) {
      if(rand_data[i] < my_min) {
	my_min = rand_data[i];
      }
    }
    
#pragma omp critical 
    {
      if(my_max > max_rand) max_rand = my_max;
      if(my_min < min_rand) min_rand = my_min;
    }
  }

#pragma omp parallel for 
  for(stype i = 0; i < no_stream; i++) {
    data[i] = ((rand_data[i] - min_rand) / (max_rand - min_rand)) * (no_unique - 1);
    D(
      if(data[i] >= no_unique || data[i] < 0) {
	std::cout << "ERROR: " << rand_data[i] << " " << min_rand << " " << max_rand << " " << data[i] << std::endl;
	
      }
    );
  }
}

else if(distNum == POISSON)
{


  #pragma omp parallel num_threads(16)
  {
    int id = omp_get_thread_num();
    std::random_device rd; 
    std::mt19937 gen((rd() + id) * (id + 11111)); 
    std::normal_distribution<> ed(no_unique / 2, 1000);    
    
    double my_max = 0;
#pragma omp for 
    for(stype i = 0; i < no_stream; i++) {
      rand_data[i] = ed(gen);
      if(rand_data[i] > my_max) {
	my_max = rand_data[i];
      }
    }

    double my_min = 0;
#pragma omp for 
    for(stype i = 0; i < no_stream; i++) {
      if(rand_data[i] < my_min) {
	my_min = rand_data[i];
      }
    }
    
#pragma omp critical 
    {
      if(my_max > max_rand) max_rand = my_max;
      if(my_min < min_rand) min_rand = my_min;
    }
  }

#pragma omp parallel for 
  for(stype i = 0; i < no_stream; i++) {
    data[i] = ((rand_data[i] - min_rand) / (max_rand - min_rand)) * (no_unique - 1);
    D(
      if(data[i] >= no_unique || data[i] < 0) {
	std::cout << "ERROR: " << rand_data[i] << " " << min_rand << " " << max_rand << " " << data[i] << std::endl;
	
      }
    );
  }
 }
}
void generateFreq(dtype* &data, std::pair<stype, dtype>* &freq_decreasing, 
      unsigned long long no_stream, unsigned long long no_unique) 
      {

  std::cout << "Computing real frequencies " << std::endl;

  freq_decreasing = new std::pair<stype, dtype>[no_unique];
#pragma omp parallel for schedule(static)
  for(dtype i = 0; i < no_unique; i++) {
    freq_decreasing[i] = std::make_pair(0, i);
  }

  for(stype i = 0; i < no_stream; i++) {
    freq_decreasing[data[i]].first++;
  }
  
  std::nth_element(freq_decreasing, freq_decreasing + TOP, freq_decreasing + no_unique, std::greater<std::pair<int,int>>());
  std::sort(freq_decreasing, freq_decreasing + TOP, std::greater<std::pair<int,int>>()); 

  D(
    std::cout << "Top frequencies: " << std::endl;
    for(unsigned i = 0; i < TOP; i++) {
      std::cout << i << "\t" << freq_decreasing[i].second << "\t" << freq_decreasing[i].first << std::endl;
    }
    std::cout << std::endl;
  );
}

#endif
