#ifndef H_GENERATOR
#define H_GENERATOR

#include <iostream>
#include <omp.h>
#include <random>
#include <algorithm>
#include <climits>
#include "common.h"

using namespace std;

std::random_device rd;
std::mt19937 e2(rd());
std::uniform_real_distribution<> dist(0, 1);



int zipf(double alpha, int n)
{
  static int first = 1;      // Static first time flag
  static double c = 0;          // Normalization constant
  static double *sum_probs;     // Pre-calculated sum of probabilities
  double z;                     // Uniform random number (0 < z < 1)
  int zipf_value;               // Computed exponential value to be returned
  int    i;                     // Loop counter
  int low, high, mid;           // Binary-search bounds

  // Compute normalization constant on first call only
  if (first == 1)
    {
      for (i=1; i<=n; i++)
	c = c + (1.0 / pow((double) i, alpha));
      c = 1.0 / c;

      sum_probs = (double*) malloc((n+1)*sizeof(*sum_probs));
      sum_probs[0] = 0;
      for (i=1; i<=n; i++) {
	sum_probs[i] = sum_probs[i-1] + c / pow((double) i, alpha);
      }
      first = 0;
    }

  // Pull a uniform random number (0 < z < 1)
  do
    {
      z = dist(e2);
    }
  while ((z == 0) || (z == 1));

  // Map z to the value
  low = 1, high = n, mid;
  do {
    mid = floor((low+high)/2);
    if (sum_probs[mid] >= z && sum_probs[mid-1] < z) {
      zipf_value = mid;
      break;
    } else if (sum_probs[mid] >= z) {
      high = mid-1;
    } else {
      low = mid+1;
    }
  } while (low <= high);

  // Assert that zipf_value is between 1 and N
  //  assert((zipf_value >=1) && (zipf_value <= n));

  return(zipf_value);
}


void generateData(dtype* &data, unsigned long long no_stream, unsigned long long no_unique,int distNum) {
  data = new dtype[no_stream];  

  double max_rand = 0, min_rand = INT_MAX;
  double* rand_data = new double[no_stream];
if(distNum == NORMAL)
{
  //  cout << "normal " << endl;
  double alp = 3;
  zipf(alp, no_unique);
#pragma omp parallel num_threads(16)
  {
    int id = omp_get_thread_num();
    /*std::random_device rd; 
    std::mt19937 gen((rd() + id) * (id + 11111)); 
    std::normal_distribution<> ed(normal_dist_mean,normal_dist_var);    */
    
    double my_max = 0;
   #pragma omp for 
    for(stype i = 0; i < no_stream; i++) {
      rand_data[i] = zipf(alp, no_unique);//ed(gen);
      if(rand_data[i] > my_max) {
	my_max = rand_data[i];
      }
    }

    //#pragma omp critical 
    //cout << "veni " << id << endl;

    double my_min = INT_MAX;
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
  /*
  int* counts = new int[no_unique];
  memset(counts, 0, sizeof(int) * no_unique);
  for(stype i = 0; i < no_stream; i++) {
    counts[data[i]]++;
  }
  
  int nz_count = 0, z_count = 0;
  for(int i = 0; i < no_unique; i++) {
    if(counts[i] == 0) {
      z_count++;
    } else {
      nz_count++;
    }
  }

  std::sort(counts, counts + no_unique, std::greater<int>()); 
  for(int i = 0; i < 200; i++) {
    cout << i << ": " << counts[i] << endl;
  }
  cout << "Z: " << z_count << " - NZ: " << nz_count << " - T: " << no_unique << endl;*/
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

//  std::cout << "Computing real frequencies " << std::endl;

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

 /* D(
    std::cout << "Top frequencies: " << std::endl;
    for(unsigned i = 0; i < TOP; i++) {
      std::cout << i << "\t" << freq_decreasing[i].second << "\t" << freq_decreasing[i].first << std::endl;
    }
    std::cout << std::endl;
  );*/
}

#endif
