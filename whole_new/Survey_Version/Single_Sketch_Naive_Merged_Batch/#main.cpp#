#include <iostream>
#include <random>
#include <algorithm>
#include <vector>
#include <string.h>
#include "csketch.hpp"
#include "common.h"
//#include "_csketch.hpp"
#include "csketch_merged.hpp"
#include "hasher.hpp"
#include "hasher_merged.hpp"
#include "generator.h"

#include "omp.h"

#define CERROR

unsigned long long no_stream, no_unique;
unsigned no_max_threads;
double epsilon, delta;
int distNum;
double zipfAlp;
int parseInputs(int argc, char** argv) {
  if(argc >= 6) {
    no_stream = pow(2, atoi(argv[1]));
    no_unique = pow(2, atoi(argv[2]));
    epsilon = atof(argv[3]);
    delta = atof(argv[4]);
    no_max_threads  = atoi(argv[5]);
    distNum=atoi(argv[6]);
    zipfAlp = atof(argv[7]);
    return 0;
  } else {
    return -1;
  }
}

void experiment_single_table(CSketch<stype, dtype>& csketch, dtype* data, std::pair<stype, dtype>* &freq_decreasing) {
  double* times = new double[no_max_threads + 1];
  memset(times, 0, sizeof(double) * (no_max_threads + 1));
  
  for(unsigned nt = 1; nt <= no_max_threads; nt *= 2) {
    csketch.reset();
    double t1 = omp_get_wtime();
#pragma omp parallel for schedule(guided,256) num_threads(nt)
    for(unsigned long long i = 0; i < no_stream; i++) {
      csketch.insert(data[i]);
    }

    times[nt] = omp_get_wtime() - t1;
    std::cout << "no threads:. " << nt << "\ttime:" << times[nt] << "\tthrough:" << no_stream / times[nt];
#ifdef CERROR
    std::cout << "\terror:" << csketch.getError(freq_decreasing, TOP) << "\ttop_item(id:" <<  freq_decreasing[0].second << "\treal:" << freq_decreasing[0].first << "\tquery:" << csketch.query(freq_decreasing[0].second) << ")";
#endif
    std::cout<< std::endl;
  }
  std::cout<< std::endl;
  delete [] times;
}

void experiment_single_table_m(CSketch_M<stype, dtype>& csketch, dtype* data, std::pair<stype, dtype>* &freq_decreasing) {
  double* times = new double[no_max_threads + 1];
  memset(times, 0, sizeof(double) * (no_max_threads + 1));
  
  for(unsigned nt = 1; nt <= no_max_threads; nt *= 2) {
    csketch.reset();
    double t1 = omp_get_wtime();
#pragma omp parallel num_threads(nt)
    {
      uint32_t results_m[256];
#pragma omp for schedule(guided,256)
      for(unsigned long long i = 0; i < no_stream; i++) {
	csketch.insert(data[i], results_m);
      }
    }

    times[nt] = omp_get_wtime() - t1;
    std::cout << "no threads:. " << nt << "\ttime:" << times[nt] << "\tthrough:" << no_stream / times[nt];
#ifdef CERROR
    std::cout << "\terror:"<< csketch.getError(freq_decreasing, TOP) << "\ttop_item(id:" <<  freq_decreasing[0].second << "\treal:" << freq_decreasing[0].first << "\tquery:" << csketch.query(freq_decreasing[0].second) << ")";
#endif
    std::cout<< std::endl;
  }
  std::cout<< std::endl;
  delete [] times;
}


void experiment_single_table_mb(CSketch_M<stype, dtype>& csketch, dtype* data, std::pair<stype, dtype>* &freq_decreasing) {
  double *times = new double[no_max_threads + 1];
  memset(times, 0, sizeof(double) * (no_max_threads +1));
  
  const unsigned batch_size = 1024;
  unsigned no_batch = no_stream / batch_size;
  unsigned no_cols = csketch.get_no_cols();

  uint32_t g_results_m[8 * batch_size * no_max_threads];    
    

  for(unsigned nt = 1; nt <= no_max_threads; nt *= 2) {
    csketch.reset();
    double t1 = omp_get_wtime();

    for(unsigned b = 0; b < no_batch/nt; b++) {
#pragma omp parallel num_threads(nt) 
      {
	int tid = omp_get_thread_num();
	uint32_t* results_m = g_results_m + (tid * (8 * batch_size));
	
	unsigned long long start = b * (batch_size * nt) + (tid * batch_size);
	unsigned long long end = start + batch_size;
	for(unsigned long long i = start; i < end; i++) {
	  csketch.get_hash()->hash(data[i], results_m);
	  results_m += 8;
	}
	//}

      //#pragma omp parallel for schedule(static, 1) num_threads(nt) 

	//#pragma omp barrier      	
#pragma omp for schedule(static, 1)  
      for(unsigned k = 0; k < 8; k++) {                          
	for(unsigned i = 0; i < (batch_size * nt); i++) {                                                                                                              
	  unsigned col_id = (g_results_m[(i * 8) + k]) % no_cols;        
	  csketch.add(k, col_id, 1); 
	}                                                                                                                                                          
      }
    }
    }
    times[nt] = omp_get_wtime() - t1;
    std::cout << "no threads:." << nt << "\ttime:" << times[nt] << "\tthrough:" << no_stream / times[nt];
#ifdef CERROR
    std::cout << "\terror:" << csketch.getError(freq_decreasing, TOP) << "\ttop_item(id:" <<  freq_decreasing[0].second << "\treal:" << freq_decreasing[0].first << "\tquery:" << csketch.query(freq_decreasing[0].second) << ")";
#endif
    std::cout << std::endl;
  }
  
  std::cout << std:: endl;
  delete [] times;
}

int main(int argc, char** argv) {
  
  if(parseInputs(argc, argv) == -1) {
    std::cout << "Wrong number of arguments" << std::endl;
    std::cout << "Usage: executable [log2(stream size)] [log2(universal set size)] [epsilon] [delta] [no max threads] [distribution no] " << std::endl;
    std::cout << "\tNORMAL 1\n\tUNIFORM 2\n\tPOISSON 3\n\tEXPONENTIAL 4" << std::endl;
    return -1;
  }

  dtype* data;
  generateData(data, no_stream, no_unique, distNum, zipfAlp);
  
#ifdef CERROR
  std::pair<stype, dtype>* freq_decreasing;
  generateFreq(data, freq_decreasing, no_stream, no_unique);
#endif

  std::cout << "No of elements in stream : " << no_stream << std::endl;

  CountMinSketch<stype, dtype> cms(epsilon, delta);

  std::cout << "Size of the sketch: " << cms.get_no_rows()  << " by " << cms.get_no_cols() <<  std::endl;
  
  unsigned no_hashes = cms.get_no_rows();
  TabularHash* h_hashes = new TabularHash[no_hashes];
  TabularHash* g_hashes = new TabularHash[no_hashes];
  cms.set_hashes(h_hashes);
  TabularHash* k_hashes = new TabularHash[no_hashes];
  TabularHashMerged* h_hash_merged = new TabularHashMerged();
  h_hash_merged->setHashTables(h_hashes);//replaced h with k
  CountMinSketch_M<stype, dtype> cms_m(epsilon, delta); 
  cms_m.set_hash(h_hash_merged);

  std::cout<<"Experimenting with single table " <<endl;
  
  std::cout << "Single Table Naive Version" << std::endl;
  experiment_single_table(cms,data,freq_decreasing);
  std::cout << "Single Table Merged Tabulation" << std::endl;
  experiment_single_table_m(cms_m,data,freq_decreasing);
  //std::cout<<"Single Table Merged Batched" <<endl;
  //experiment_single_table_mb(cms_m,data,freq_decreasing);
  
  delete [] data;
#ifdef CERROR
  delete [] freq_decreasing;
#endif
  return 0;
}
