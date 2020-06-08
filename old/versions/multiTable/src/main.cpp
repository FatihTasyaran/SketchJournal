#include <iostream>
#include <random>
#include <algorithm>
#include <vector>
#include <string.h>

#include "common.h"
#include "csketch.hpp"
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
      CSketch_M<stype, dtype>* my_sketch =  csketches[omp_get_thread_num()]; 
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
***
void experiment_single_table_m(CSketch_M<stype, dtype>& csketch_m, dtype* data, std::pair<stype, dtype> *freq_decreasing) {
  double *times = new double[no_max_threads + 1];
  memset(times, 0, sizeof(double) * (no_max_threads + 1));
  unsigned **buffers;

  for(unsigned nt = 1; nt <= no_max_threads; nt *= 2) {
    csketch_m.reset();
    double t1 = omp_get_wtime();
    *buffers = new *unsigned[nt];
    for(i = 0; i < (num_threads - 1)) {
      buffers[i] = new unsigned[8000];
    }
#pragma omp parallel for schedule(guided, 256) num_threads(nt)
    for(unsigned long long i = 0; i < no_stream;){
      for(bi = 0; bi < 1000 ; bi++){
        unsigned val[8];
        val = csketch_m.hashElement(data[i])
        buffers[omp_get_thread_num][bi] = val[0];
        buffers[omp_get_thread_num][bi + 1] = val[1];
        buffers[omp_get_thread_num][bi + 2] = val[2];
        buffers[omp_get_thread_num][bi + 3] = val[3];
        buffers[omp_get_thread_num][bi + 4] = val[4];
        buffers[omp_get_thread_num][bi + 5] = val[5];
        buffers[omp_get_thread_num][bi + 6] = val[6];
        buffers[omp_get_thread_num][bi + 7] = val[7];
        i++;
      }
      for(bi = 0; bi < 8000 - 1; bi++){
        csketch_m
      }

    }
      }


}
*/



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

  TabularHashMerged* h_hash_merged = new TabularHashMerged();
  h_hash_merged->setHashTables(h_hashes);
  CountMinSketch_M<stype, dtype> cms_m(epsilon, delta); 
  cms_m.set_hash(h_hash_merged);


  /*********************************************************************************/
  
  std::cout << " Experimenting with multiple sketches distributed to the threads " << std::endl;
  
  std::vector<CSketch<stype, dtype>*> mult_cms;
  for(unsigned i = 0; i < no_max_threads; i++) {
    CountMinSketch<stype, dtype>* tmp = new CountMinSketch<stype, dtype>(epsilon, delta);
    tmp->set_hashes(h_hashes);
    mult_cms.push_back(tmp);
  }
  std::cout << "Without Merged Hash Tables " << std::endl;
  experiment_dist_sketch(mult_cms, cms, data, freq_decreasing);

  std::vector<CSketch_M<stype, dtype>*> mult_cms_m;
  for(unsigned i = 0; i < no_max_threads; i++) {
    CountMinSketch_M<stype, dtype>* tmp = new CountMinSketch_M<stype, dtype>(epsilon, delta);
    tmp->set_hash(h_hash_merged);
    mult_cms_m.push_back(tmp);
  }
  std::cout << "With Merged Hash Tables " << std::endl;
  experiment_dist_sketch_m(mult_cms_m, cms_m, data, freq_decreasing);

  
  delete [] data;
#ifdef CERROR
  delete [] freq_decreasing;
#endif
  return 0;
}
