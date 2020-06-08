#ifndef H_GENERATOR_SEQ
#define H_GENERATOR_SEQ

#include <iostream>
#include <omp.h>
#include <random>
#include <algorithm>
#include "common.h"

using namespace std;



void generateData(dtype* &data, unsigned long long no_stream, unsigned long long no_unique,int distNum) {
  data = new dtype[no_stream];


  double* rand_data = new double[no_stream];
  static double my_max = 0;
  static double my_min = 0;
if(distNum == NORMAL)
{
	int id = 0;
	std::random_device rd;
	for ( int i = 0; i < 16; i++ )
	{
		std::mt19937 gen(rd() + id) * ( id + 11111));
		std::normal_distribution<> ed(normal_dist_mean, normal_dist_var);

		
		
		for (stype i = 0; i < no_stream, i++) 
		{
			rand_data[i] = ed(gen);
			if(rand_data[i] > my_max)
			{
				my_max = rand_data[i];
			}
			if(rand_data[i] < my_min)
			{
				my_min = rand_data[i];
			}
		}

	}
	  
	for(stype i = 0; i < no_stream; i++) {
    data[i] = ((rand_data[i] - my_min) / (my_max - my_min)) * (no_unique - 1);
	D(
      if(data[i] >= no_unique || data[i] < 0) {
	std::cout << "ERROR: " << rand_data[i] << " " << min_rand << " " << max_rand << " " << data[i] << std::endl;

      }
    );
	}

	}



else if(distNum == UNIFORM)
{
	int id = 0;
	std::random_device rd;
	for ( int i = 0; i < 16; i++ )
	{
		std::mt19937 gen(rd() + id) * ( id + 11111));
		std::uniform_int_distribution<> ed(uniform_param_low, uniform param upper);

		
		
		for (stype i = 0; i < no_stream, i++) 
		{
			rand_data[i] = ed(gen);
			if(rand_data[i] > my_max)
			{
				my_max = rand_data[i];
			}
			if(rand_data[i] < my_min)
			{
				my_min = rand_data[i];
			}
		}

	}
	  
	for(stype i = 0; i < no_stream; i++) {
    data[i] = ((rand_data[i] - my_min) / (my_max - my_min)) * (no_unique - 1);
	D(
      if(data[i] >= no_unique || data[i] < 0) {
	std::cout << "ERROR: " << rand_data[i] << " " << min_rand << " " << max_rand << " " << data[i] << std::endl;

      }
    );
	}

}

void generateFreq(dtype* &data, std::pair<stype, dtype>* &freq_decreasing,
      unsigned long long no_stream, unsigned long long no_unique)
      {

  std::cout << "Computing real frequencies " << std::endl;

  freq_decreasing = new std::pair<stype, dtype>[no_unique];
//
  for(dtype i = 0; i < no_unique; i++) {
    freq_decreasing[i] = std::make_pair(0, i);
  }

  for(stype i = 0; i < no_stream; i++) {
    freq_decreasing[data[i]].first++;
  }

  std::nth_element(freq_decreasing, freq_decreasing + TOP, freq_decreasing + no_unique, std::greater<std::pair<int,int>>());
  std::sort(freq_decreasing, freq_decreasing + TOP, std::greater<std::pair<int,int>>());


}




#endif