#include <iostream>
#include <random>
#include <algorithm>
#include <vector>
#include <string.h>
#include "tw_hasher.hpp"
#include "csketch.hpp"
#include "common.h"
//#include "_csketch.hpp"
#include "csketch_merged.hpp"
#include "csketch_merged_padded.hpp"
#include "hasher.hpp"
#include "hasher_merged.hpp"
#include "generator.h"

#include "omp.h"

#define CERROR

unsigned long long no_stream, no_unique;
unsigned no_max_threads;
double epsilon, delta;
int distNum;
int parseInputs(int argc, char** argv) {
  if(argc >= 6) {
    no_stream = pow(2, atoi(argv[1]));
    no_unique = pow(2, atoi(argv[2]));
    epsilon = atof(argv[3]);
    delta = atof(argv[4]);
    no_max_threads  = atoi(argv[5]);
    distNum=atoi(argv[6]);
    return 0;
  } else {
    return -1;
  }
}

void experiment_dist_sketch(std::vector<CSketch<stype, dtype>*>& csketches, CSketch<stype, dtype>& reduce_sketch, dtype* data, std::pair<stype, dtype>* &freq_decreasing) {
  double* times = new double[no_max_threads + 1];
  memset(times, 0, sizeof(double) * (no_max_threads + 1));
  
  for(unsigned nt = 1; nt <= no_max_threads; nt *= 2) {
    for(unsigned t = 0; t < nt; t++) {
      csketches[t]->reset();
    }
    
    double t1 = omp_get_wtime();
#pragma omp parallel num_threads(nt)
    {
      CSketch<stype, dtype>* my_sketch =  csketches[omp_get_thread_num()]; 
#pragma omp for schedule(guided,256) 
      for(unsigned long long i = 0; i < no_stream; i++) {
	my_sketch->insert(data[i]);
      }
    }
    times[nt] = omp_get_wtime() - t1;
    std::cout << "\tNo thr.: " << nt << "\ttime: " << times[nt] << "\tthrough.: " << no_stream / times[nt];

    reduce_sketch.reset();
    for(unsigned tid = 0; tid < nt; tid++) {
      for(unsigned r = 0; r < reduce_sketch.get_no_rows(); r++) {
	for(unsigned c = 0; c < reduce_sketch.get_no_cols(); c++) {
	  reduce_sketch.add(r, c, csketches[tid]->get(r, c));
	}
      }
    }
#ifdef CERROR
    std::cout << "\terr:" << reduce_sketch.getError(freq_decreasing, TOP) << "\ttop_item(id:" <<
      freq_decreasing[0].second << "\treal:" << freq_decreasing[0].first << "\tquery:" << reduce_sketch.query(freq_decreasing[0].second) << ")";
#endif    
    std::cout<< std::endl;
  }
  std::cout<< std::endl;
  delete [] times;
}


void experiment_dist_sketch_m(std::vector<CSketch_M<stype, dtype>*>& csketches, CSketch_M<stype, dtype>& reduce_sketch, dtype* data, std::pair<stype, dtype>* &freq_decreasing) {
  double* times = new double[no_max_threads + 1];
  memset(times, 0, sizeof(double) * (no_max_threads + 1));
  
  for(unsigned nt = 1; nt <= no_max_threads; nt *= 2) {
    for(unsigned t = 0; t < nt; t++) {
      csketches[t]->reset();
    }
    
    double t1 = omp_get_wtime();
#pragma omp parallel num_threads(nt)
    {
      uint32_t results_m[256];
      CSketch_M<stype, dtype>* my_sketch =  csketches[omp_get_thread_num()]; 
#pragma omp for schedule(guided,256) 
      for(unsigned long long i = 0; i < no_stream; i++) {
	my_sketch->insert(data[i], results_m);
      }
    }
    times[nt] = omp_get_wtime() - t1;
    std::cout << "\tNo thr.: " << nt << "\ttime: " << times[nt] << "\tthrough.: " << no_stream / times[nt];

    reduce_sketch.reset();
    for(unsigned tid = 0; tid < nt; tid++) {
      for(unsigned r = 0; r < reduce_sketch.get_no_rows(); r++) {
	for(unsigned c = 0; c < reduce_sketch.get_no_cols(); c++) {
	  reduce_sketch.add(r, c, csketches[tid]->get(r, c));
	}
      }
    }
#ifdef CERROR
    std::cout << "\terr:" << reduce_sketch.getError(freq_decreasing, TOP) << "\ttop_item(id:" <<
      freq_decreasing[0].second << "\treal:" << freq_decreasing[0].first << "\tquery:" << reduce_sketch.query(freq_decreasing[0].second) << ")";
#endif    
    std::cout<< std::endl;
  }
  std::cout<< std::endl;
  delete [] times;
}

void experiment_dist_row(CSketch<stype, dtype>& csketch, dtype* data, std::pair<stype, dtype>* &freq_decreasing) {
  double* times = new double[no_max_threads + 1];
  memset(times, 0, sizeof(double) * (no_max_threads + 1));
  
  for(unsigned nt = 1; nt <= no_max_threads; nt *= 2) {
    csketch.reset();
    double t1 = omp_get_wtime();
    csketch.insert_stream(data, no_stream, nt);
    times[nt] = omp_get_wtime() - t1;
    std::cout << "\tNo thr.: " << nt << "\ttime: " << times[nt] << "\tthrough.: " << no_stream / times[nt];
#ifdef CERROR
    std::cout << "\terr:" << csketch.getError(freq_decreasing, TOP) << "\ttop_item(id:" <<  freq_decreasing[0].second << "\treal:" << freq_decreasing[0].first << "\tquery:" << csketch.query(freq_decreasing[0].second) << ")";
#endif
    std::cout<< std::endl;
  }
  std::cout<< std::endl;
  delete [] times;
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
    std::cout << "\tNo thr.: " << nt << "\ttime: " << times[nt] << "\tthrough.: " << no_stream / times[nt];
#ifdef CERROR
    std::cout << "\terr:" << csketch.getError(freq_decreasing, TOP) << "\ttop_item(id:" <<  freq_decreasing[0].second << "\treal:" << freq_decreasing[0].first << "\tquery:" << csketch.query(freq_decreasing[0].second) << ")";
#endif
    std::cout<< std::endl;
  }
  std::cout<< std::endl;
  delete [] times;
}

/****Working on*****
***/
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
    std::cout << "\tNo thr.: " << nt << "\ttime: " << times[nt] << "\tthrough.: " << no_stream / times[nt];
#ifdef CERROR
    std::cout << "\terr:" << csketch.getError(freq_decreasing, TOP) << "\ttop_item(id:" <<  freq_decreasing[0].second << "\treal:" << freq_decreasing[0].first << "\tquery:" << csketch.query(freq_decreasing[0].second) << ")";
#endif
    std::cout<< std::endl;
  }
  std::cout<< std::endl;
  delete [] times;
}


//Trying on////
//////////////

void experiment_single_table_mb(CSketch_M<stype, dtype>& csketch, dtype* data, std::pair<stype, dtype>* &freq_decreasing) {
  double *times = new double[no_max_threads + 1];
  memset(times, 0, sizeof(double) * (no_max_threads +1));

  const unsigned batch_size = 1024;
  unsigned no_batch = no_stream / batch_size;
  unsigned no_cols = csketch.get_no_cols();


  uint32_t g_results_m[8 * batch_size * no_max_threads];    
    
  
  for(unsigned nt = 2; nt <= no_max_threads; nt *= 2) {
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
    std::cout << "\tNo thr.: " << nt << "\ttime: " << times[nt] << "\tthrough.: " << no_stream / times[nt];
#ifdef CERROR
    std::cout << "\terr:" << csketch.getError(freq_decreasing, TOP) << "\ttop_item(id:" <<  freq_decreasing[0].second << "\treal:" << freq_decreasing[0].first << "\tquery:" << csketch.query(freq_decreasing[0].second) << ")";
#endif
    std::cout << std::endl;
  }
  
  std::cout << std:: endl;
  delete [] times;
}



void experiment_single_table_mbp(CSketch_MP<stype, dtype>& csketch, dtype* data, std::pair<stype, dtype>* &freq_decreasing) {
  double *times = new double[no_max_threads + 1];
  memset(times, 0, sizeof(double) * (no_max_threads +1));

  const unsigned batch_size = 1024;
  unsigned no_batch = no_stream / batch_size;
  unsigned no_cols = csketch.get_no_cols();

  uint32_t g_results_m[8 * batch_size * no_max_threads];    
  
  for(unsigned nt = 2; nt <= no_max_threads; nt *= 2) {
    csketch.reset();
    double t1 = omp_get_wtime();
    //int sum = 0;
    for(unsigned b = 0; b < no_batch/nt; b++) {
      
#pragma omp parallel num_threads(nt) 
      {
	int tid = omp_get_thread_num();
	uint32_t* results_m = g_results_m + (tid * (8 * batch_size));
	
	unsigned long long start = b * (batch_size * nt) + (tid * batch_size);
	unsigned long long end = start + batch_size;
	for(unsigned long long i = start; i < end; i++) {
	  csketch.get_hash()->hash(data[i], results_m);//, no_cols);
	  results_m += 8;
	}
	//}

      //#pragma omp parallel for schedule(static, 1) num_threads(nt) 

	//#pragma omp barrier      	

#pragma omp for schedule(static, 1) //reduction(+:sum)
      for(unsigned k = 0; k < 8; k++) {                          
	for(unsigned i = 0; i < (batch_size * nt); i++) {                                                                                                              
	  unsigned col_id = (g_results_m[(i * 8) + k]) % no_cols;        
	  //	  sum += col_id;
	  //	  if(col_id >= no_cols) {cout << col_id << " " << no_cols << " aaaaa" << endl;}
	  csketch.add(k, col_id, 1); 
	}                                                                                                                                                          
      }
      }
    }
    times[nt] = omp_get_wtime() - t1;
    std::cout << "\tNo thr.: " << nt << "\ttime: " << times[nt] << "\tthrough.: " << no_stream / times[nt];
#ifdef CERROR
    std::cout << "\terr:" << csketch.getError(freq_decreasing, TOP) << "\ttop_item(id:" <<  freq_decreasing[0].second << "\treal:" << freq_decreasing[0].first << "\tquery:" << csketch.query(freq_decreasing[0].second) << ")";
#endif
    std::cout << std::endl;
    // cout << sum << endl;
      
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
  generateData(data, no_stream, no_unique,distNum);
  
#ifdef CERROR
  std::pair<stype, dtype>* freq_decreasing;
  generateFreq(data, freq_decreasing, no_stream, no_unique);
#endif

  std::cout << "No of elements in stream : " << no_stream << std::endl;

  CountMinSketch<stype, dtype> cms(epsilon, delta);
  CountMinSketchRestricted<stype, dtype> cmsr(epsilon, delta);  
  CountMinMeanSketch<stype, dtype> cmms(epsilon, delta);  
  CountSketch<stype, dtype> cs(epsilon, delta);  

  std::cout << "Size of the sketch: " << cs.get_no_rows()  << " by " << cs.get_no_cols() <<  std::endl;
  
  unsigned no_hashes = cs.get_no_rows();
  TabularHash* h_hashes = new TabularHash[no_hashes];
  TabularHash* g_hashes = new TabularHash[no_hashes];
  cms.set_hashes(h_hashes);
  cmsr.set_hashes(h_hashes);
  cmms.set_hashes(h_hashes);
  cs.set_hashes(h_hashes);
  cs.set_g_hashes(g_hashes);
  TabularHash* k_hashes = new TabularHash[no_hashes];
  TabularHashMerged* h_hash_merged = new TabularHashMerged();
  h_hash_merged->setHashTables(h_hashes);//replaced h with k
  CountMinSketch_M<stype, dtype> cms_m(epsilon, delta); 
  cms_m.set_hash(h_hash_merged);
  CountMinSketch_MP<stype, dtype> cms_mp(epsilon, delta); 
  cms_mp.set_hash(h_hash_merged);

  /*********************************************************************************/
  
  /*  std::cout << " Experimenting with a shared single table " << std::endl;
  experiment_single_sketch(cms, data, freq_decreasing);
  experiment_single_sketch(cmms, data, freq_decreasing);
  experiment_single_sketch(cs, data, freq_decreasing);*/
  
  /*********************************************************************************/
  
  /*  std::cout << " Experimenting with a single sketch, rows are distributed to threads " << std::endl;
  experiment_dist_row(cms, data, freq_decreasing);
  experiment_dist_row(cmms, data, freq_decreasing);
  experiment_dist_row(cs, data, freq_decreasing);*/

  /*********************************************************************************/
  //std::cout << " Experimenting with multiple sketches distributed to the threads " << std::endl;
  //std::cout<<"Experimenting with single table " <<endl;

  std::vector<CSketch_M<stype, dtype>*> mult_cms_m;
  for(unsigned i = 0; i < no_max_threads; i++) {
    CountMinSketch_M<stype, dtype>* tmp = new CountMinSketch_M<stype, dtype>(epsilon, delta);
    tmp->set_hash(h_hash_merged);
    mult_cms_m.push_back(tmp);
  }
  experiment_dist_sketch_m(mult_cms_m, cms_m, data, freq_decreasing);
  std::vector<CSketch<stype, dtype>*> mult_cms;
  for(unsigned i = 0; i < no_max_threads; i++) {
    CountMinSketch<stype, dtype>* tmp = new CountMinSketch<stype, dtype>(epsilon, delta);
    tmp->set_hashes(h_hashes);
    mult_cms.push_back(tmp);
  }  
  experiment_dist_sketch(mult_cms, cms, data, freq_decreasing);

  
  //  std::cout << " Experimenting With Single Table " << std::endl;
  //experiment_single_table(cms,data,freq_decreasing);
  
  //std::cout << "Experimenting With Single Table Using Merged Hash Table " << std::endl;
  //experiment_single_table_m(cms_m,data,freq_decreasing);
  
    
  //  experiment_dist_sketch(mult_cms, cms, data, freq_decreasing);
  // std::cout<<"Experimenting with batch " <<endl;
  //experiment_single_table_mb(cms_m,data,freq_decreasing);
  // experiment_single_table_mbp(cms_mp,data,freq_decreasing);
  
  /*  std::vector<CSketch<stype, dtype>*> mult_cmsr;
  for(unsigned i = 0; i < no_max_threads; i++) {
    CountMinSketchRestricted<stype, dtype>* tmp = new CountMinSketchRestricted<stype, dtype>(epsilon, delta);
    tmp->set_hashes(h_hashes);
    mult_cmsr.push_back(tmp);
  }
  experiment_dist_sketch(mult_cmsr, cmsr, data, freq_decreasing);

  std::vector<CSketch<stype, dtype>*> mult_cmms;
  for(unsigned i = 0; i < no_max_threads; i++) {
    CountMinMeanSketch<stype, dtype>* tmp = new CountMinMeanSketch<stype, dtype>(epsilon, delta);
    tmp->set_hashes(h_hashes);
    mult_cmms.push_back(tmp);
  }
  experiment_dist_sketch(mult_cmms, cmms, data, freq_decreasing);
  
  std::vector<CSketch<stype, dtype>*> mult_cs;
  for(unsigned i = 0; i < no_max_threads; i++) {
    CountSketch<stype, dtype>* tmp = new CountSketch<stype, dtype>(epsilon, delta);
    tmp->set_hashes(h_hashes);
    tmp->set_g_hashes(g_hashes);
    mult_cs.push_back(tmp);
  }
  experiment_dist_sketch(mult_cs, cs, data, freq_decreasing);*/
  /*********************************************************************************/
  delete [] data;
#ifdef CERROR
  delete [] freq_decreasing;
#endif
  return 0;
}
