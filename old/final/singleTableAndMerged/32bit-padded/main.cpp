#include <iostream>
#include <random>
#include <algorithm>
#include <vector>
#include <string.h>


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





void experiment_single_table_mb(CSketch_M<stype, dtype>& csketch, dtype* data, std::pair<stype, dtype>* &freq_decreasing) {
  double *times = new double[no_max_threads + 1];
  double *htimesthread = new double[no_max_threads + 1];
  double *htimestotal = new double[no_max_threads + 1];
  double *wtimesthread = new double[no_max_threads + 1];
  double *wtimestotal = new double[no_max_threads + 1];

  memset(times, 0, sizeof(double) * (no_max_threads +1));
  memset(htimestotal, 0, sizeof(double) * (no_max_threads +1));
  memset(wtimestotal, 0, sizeof(double) * (no_max_threads +1));
 

  const unsigned batch_size = 1024;
  unsigned no_batch = no_stream / batch_size;
  unsigned no_cols = csketch.get_no_cols();

  TabularHashMerged *separate_hash = new TabularHashMerged;
  uint32_t g_results_m[8 * batch_size * no_max_threads];    
    
  for(unsigned nt = 1; nt <= no_max_threads; nt *= 2) {
    csketch.reset();
    memset(htimesthread, 0, sizeof(double) * (no_max_threads +1));
    memset(wtimesthread, 0, sizeof(double) * (no_max_threads +1));
    double t1 = omp_get_wtime();
    for(unsigned b = 0; b < no_batch/nt; b++) {
#pragma omp parallel num_threads(nt) 
      {
	int tid = omp_get_thread_num();
	uint32_t* results_m = g_results_m + (tid * (8 * batch_size));
  double cp1, cp2, htimethread, wtimethread;

  cp1 = omp_get_wtime();
	
	unsigned long long start = b * (batch_size * nt) + (tid * batch_size);
	unsigned long long end = start + batch_size;

  for(unsigned long long i = start; i < end; i++) {

    separate_hash->hash(data[i], results_m);
	  results_m += 8;
	}
  htimethread = omp_get_wtime() - cp1;
  
  htimesthread[tid] += htimethread;
  
	//}
      //#pragma omp parallel for schedule(static, 1) num_threads(nt) 
	
	//#pragma omp barrier      	
  
  cp2 = omp_get_wtime();
#pragma omp for schedule(static, 1)  
      for(unsigned k = 0; k < 8; k++) {                          
	for(unsigned i = 0; i < (batch_size * nt); i++) {                                                                                                              
	  unsigned col_id = (g_results_m[(i * 8) + k]) % no_cols;        
	  csketch.add(k, col_id, 1); 
	}                                                                                                                                                          
      }
      wtimethread = omp_get_wtime() - cp2;

      wtimesthread[tid] += wtimethread;
    }
    }
    times[nt] = omp_get_wtime() - t1;
    double mymax_h = htimesthread[0];
    double mymax_w = wtimesthread[0];
    for(int j = 1; j <= no_max_threads; j++){
      if (htimesthread[j] > mymax_h){
        mymax_h = htimesthread[j];
      }
      if (wtimesthread[j] > mymax_w){
        mymax_w = wtimesthread[j];
      }
    }
    htimestotal[nt] = mymax_h;
    wtimestotal[nt] = mymax_w;
    std::cout << "\tNo thr.: " << nt << "\ttime: " << times[nt] << "\thtime:" << htimestotal[nt] << "\twtime:" << wtimestotal[nt] << " " << "\tthrough.: " << no_stream / times[nt];
#ifdef CERROR
    std::cout << "\terr:" << csketch.getError(freq_decreasing, TOP, separate_hash) << "\ttop_item(id:" <<  freq_decreasing[0].second << "\treal:" << freq_decreasing[0].first << "\tquery:" << csketch.query(freq_decreasing[0].second,separate_hash) << ")";
#endif
    std::cout << std::endl;
  }
  
  std::cout << std:: endl;
  delete [] times;
}



void experiment_single_table_mbp(CSketch_MP<stype, dtype>& csketch, dtype* data, std::pair<stype, dtype>* &freq_decreasing) {
  double *times = new double[no_max_threads + 1];
  double *htimesthread = new double[no_max_threads + 1];
  double *htimestotal = new double[no_max_threads + 1];
  double *wtimesthread = new double[no_max_threads + 1];
  double *wtimestotal = new double[no_max_threads + 1];
  
  memset(times, 0, sizeof(double) * (no_max_threads +1));
  memset(htimestotal, 0, sizeof(double) * (no_max_threads +1));
  memset(wtimestotal, 0, sizeof(double) * (no_max_threads +1));
  

  const unsigned batch_size = 1024;
  unsigned no_batch = no_stream / batch_size;
  unsigned no_cols = csketch.get_no_cols();

  TabularHashMerged *separate_hash = new TabularHashMerged;
  uint32_t g_results_m[8 * batch_size * no_max_threads];    
  
  for(unsigned nt = 1; nt <= no_max_threads; nt *= 2) {
    csketch.reset();
    memset(htimesthread, 0, sizeof(double) * (no_max_threads +1));
    memset(wtimesthread, 0, sizeof(double) * (no_max_threads +1));
    double t1 = omp_get_wtime();
    //int sum = 0;
    for(unsigned b = 0; b < no_batch/nt; b++) {
      
#pragma omp parallel num_threads(nt) 
      {
	int tid = omp_get_thread_num();
	uint32_t* results_m = g_results_m + (tid * (8 * batch_size));
  double cp1, cp2, htimethread, wtimethread;
	
  cp1 = omp_get_wtime();

	unsigned long long start = b * (batch_size * nt) + (tid * batch_size);
	unsigned long long end = start + batch_size;
	for(unsigned long long i = start; i < end; i++) {
	  separate_hash->hash(data[i], results_m);//, no_cols);
	  results_m += 8;
	}
  htimethread = omp_get_wtime() - cp1;

  //#pragma omp critical
    htimesthread[tid] += htimethread;
	//}

      //#pragma omp parallel for schedule(static, 1) num_threads(nt) 

    #pragma omp barrier      	

	cp2 = omp_get_wtime();
#pragma omp for schedule(static, 1) //reduction(+:sum)
      for(unsigned k = 0; k < 8; k++) {                          
	for(unsigned i = 0; i < (batch_size * nt); i++) {                                                                                                              
	  unsigned col_id = (g_results_m[(i * 8) + k]) % no_cols;        
	  //	  sum += col_id;
	  //	  if(col_id >= no_cols) {cout << col_id << " " << no_cols << " aaaaa" << endl;}
	  csketch.add(k, col_id, 1); 
	}                                                                                                                                                          
      }
      wtimethread = omp_get_wtime() - cp2;
      //#pragma omp critical
        wtimesthread[tid] += wtimethread;
      }
    }
    times[nt] = omp_get_wtime() - t1;
    double mymax_h = htimesthread[0];
    double mymax_w = wtimesthread[0];
    for(int j = 1; j <= no_max_threads; j++){
      if (htimesthread[j] > mymax_h){
        mymax_h = htimesthread[j];
      }
      if (wtimesthread[j] > mymax_w){
        mymax_w = wtimesthread[j];
      }
    }
    htimestotal[nt] = mymax_h;
    wtimestotal[nt] = mymax_w;
    std::cout << "\tNo thr.: " << nt << "\ttime: " << times[nt] << "\thtime:" << htimestotal[nt] << "\twtime:" << wtimestotal[nt] << " " << "\tthrough.: " << no_stream / times[nt];
#ifdef CERROR
    std::cout << "\terr:" << csketch.getError(freq_decreasing, TOP, separate_hash) << "\ttop_item(id:" <<  freq_decreasing[0].second << "\treal:" << freq_decreasing[0].first << "\tquery:" << csketch.query(freq_decreasing[0].second,separate_hash) << ")";
#endif
    std::cout << std::endl;
    // cout << sum << endl;
      
  }
  
  std::cout << std:: endl;
  delete [] times;
  }


void experiment_single_table_mbp_2(CSketch_MP<stype, dtype>& csketch, dtype* data, std::pair<stype, dtype>* &freq_decreasing) {
  double *times = new double[no_max_threads + 1];
  double *htimesthread = new double[no_max_threads + 1];
  double *htimestotal = new double[no_max_threads + 1];
  double *wtimesthread = new double[no_max_threads + 1];
  double *wtimestotal = new double[no_max_threads + 1];
  
  memset(times, 0, sizeof(double) * (no_max_threads +1));
  memset(htimestotal, 0, sizeof(double) * (no_max_threads +1));
  memset(wtimestotal, 0, sizeof(double) * (no_max_threads +1));

  const unsigned batch_size = 8192;
  unsigned no_batch = no_stream / batch_size;
  unsigned no_cols = csketch.get_no_cols();

  TabularHashMerged *separate_hash = new TabularHashMerged;
  uint32_t g_results_m[8 * batch_size * no_max_threads];    
  uint32_t g_results_m_BUFFER[8 * batch_size * no_max_threads];    
  
  uint32_t* g_results_temp;
  uint32_t* g_results_m_p = &(g_results_m[0]);
  uint32_t* g_results_m_BUFFER_p = &(g_results_m_BUFFER[0]);

  csketch.reset();
  memset(htimesthread, 0, sizeof(double) * (no_max_threads +1));
  memset(wtimesthread, 0, sizeof(double) * (no_max_threads +1));
  omp_set_nested(1);

  double t1 = omp_get_wtime();
  //int sum = 0;
  const unsigned nt = 16;
  
#pragma omp parallel num_threads(8)
  {
    int tid = omp_get_thread_num();
    uint32_t* results_m = g_results_m_BUFFER_p + (tid * (8 * batch_size));

    double cp1, htimethread;    
    cp1 = omp_get_wtime();
    unsigned long long start = (tid * batch_size);
    unsigned long long end = start + batch_size;
    for(unsigned long long i = start; i < end; i++) {
      separate_hash->hash(data[i], results_m);
      results_m += 8;
    }
    htimethread = omp_get_wtime() - cp1;
    htimesthread[tid] += htimethread;
  }

  for(unsigned b = 0; b < (no_batch/8) - 1; b++) {    
#pragma omp parallel num_threads(nt) 
    {
      int tid = omp_get_thread_num();
      if(tid < 8) {
	uint32_t* results_m = g_results_m_p + (tid * (8 * batch_size));
	double cp1, htimethread;
	
	cp1 = omp_get_wtime();	
	unsigned long long start = (b+1) * (batch_size * 8) + (tid * batch_size);
	unsigned long long end = start + batch_size;
	for(unsigned long long i = start; i < end; i++) {
	  separate_hash->hash(data[i], results_m);
	  results_m += 8;
	}
	htimethread = omp_get_wtime() - cp1;
	htimesthread[tid] += htimethread;
      } else {
	double cp2,  wtimethread;
	cp2 = omp_get_wtime();

	unsigned k = tid - 8;
	for(unsigned j = 0; j < (batch_size * 8); j++) {                                                                                                              
	  unsigned col_id = (g_results_m_BUFFER_p[(j * 8) + k]) % no_cols;        
	  csketch.add(k, col_id, 1); 
	}                                                                                                                                                         
      
	wtimethread = omp_get_wtime() - cp2;
	wtimesthread[tid] += wtimethread;
      }
    }
    
    uint32_t* g_results_temp = g_results_m_p;
    g_results_m_p = g_results_m_BUFFER_p;
    g_results_m_BUFFER_p = g_results_temp;
  }

#pragma omp parallel num_threads(8)
  {
    int tid = omp_get_thread_num();

    double cp2,  wtimethread;
    cp2 = omp_get_wtime();
#pragma omp for schedule(static, 1) //reduction(+:sum) 
    for(unsigned k = 0; k < 8; k++) {
      for(unsigned i = 0; i < (batch_size * 8); i++) {
	unsigned col_id = (g_results_m_BUFFER_p[(i * 8) + k]) % no_cols;
	csketch.add(k, col_id, 1);
      }
    }
    wtimethread = omp_get_wtime() - cp2;
    wtimesthread[tid] += wtimethread;
  }

  times[nt] = omp_get_wtime() - t1;
  double mymax_h = htimesthread[0];
  double mymax_w = wtimesthread[0];
  for(int j = 1; j <= no_max_threads; j++){
    if (htimesthread[j] > mymax_h){
      mymax_h = htimesthread[j];
    }
    if (wtimesthread[j] > mymax_w){
      mymax_w = wtimesthread[j];
    }
  }
  htimestotal[nt] = mymax_h;
  wtimestotal[nt] = mymax_w;
  std::cout << "\tNo thr.: " << nt << "\ttime: " << times[nt] << "\thtime:" << htimestotal[nt] << "\twtime:" << wtimestotal[nt] << " " << "\tthrough.: " << no_stream / times[nt];
#ifdef CERROR
  std::cout << "\terr:" << csketch.getError(freq_decreasing, TOP, separate_hash) << "\ttop_item(id:" <<  freq_decreasing[0].second << "\treal:" << freq_decreasing[0].first << "\tquery:" << csketch.query(freq_decreasing[0].second,separate_hash) << ")";
#endif
  std::cout << std::endl;
  
  std::cout << std:: endl;
  delete [] times;
  delete [] htimestotal;
  delete [] wtimestotal;
  delete [] htimesthread;
  delete [] wtimesthread;
}

void experiment_single_table_mbp16(CSketch_MP<stype, dtype>& csketch, dtype* data, std::pair<stype, dtype>* &freq_decreasing) {
  double *times = new double[no_max_threads + 1];
  double *htimesthread = new double[no_max_threads + 1];
  double *htimestotal = new double[no_max_threads + 1];
  double *wtimesthread = new double[no_max_threads + 1];
  double *wtimestotal = new double[no_max_threads + 1];
  
  memset(times, 0, sizeof(double) * (no_max_threads +1));
  memset(htimestotal, 0, sizeof(double) * (no_max_threads +1));
  memset(wtimestotal, 0, sizeof(double) * (no_max_threads +1));

  const unsigned batch_size = 1024;
  unsigned no_batch = no_stream / batch_size;
  unsigned no_cols = csketch.get_no_cols();
  unsigned half = no_cols / 2;

  unsigned buffer[16][batch_size * 16];
  unsigned counter[16];
  std::fill_n(&counter[0], 16, 0);

  TabularHashMerged *separate_hash = new TabularHashMerged;
  uint32_t g_results_m[8 * batch_size * no_max_threads];    
  
  unsigned nt = 16;

  csketch.reset();
  memset(htimesthread, 0, sizeof(double) * (no_max_threads +1));
  memset(wtimesthread, 0, sizeof(double) * (no_max_threads +1));
  double t1 = omp_get_wtime();

  for(unsigned b = 0; b < no_batch/nt; b++) {    
#pragma omp parallel num_threads(16) 
    {
      int tid = omp_get_thread_num();
      uint32_t* results_m = g_results_m + (tid * (8 * batch_size));
      double cp1, cp2, htimethread, wtimethread;
      counter[tid] = 0;
            
      cp1 = omp_get_wtime();
      unsigned long long start = b * (batch_size * nt) + (tid * batch_size);
      unsigned long long end = start + batch_size;
      for(unsigned long long i = start; i < end; i++) {
	separate_hash->hash(data[i], results_m);//, no_cols);
	results_m += 8;
      }
      htimethread = omp_get_wtime() - cp1;
      htimesthread[tid] += htimethread;
      
#pragma omp barrier      	

      cp2 = omp_get_wtime();
#pragma omp for schedule(static) //reduction(+:sum)
      for(unsigned i = 0; i < 8 * batch_size * no_max_threads; i++){
	unsigned col_id = g_results_m[i] % no_cols;
	unsigned row = i % 8;
	unsigned bufid = 2 * row;

	if(col_id < half){
	  buffer[bufid][counter[bufid]++] = col_id;
	} else {
	  buffer[bufid+1][counter[bufid+1]++] = col_id;
	}
      }

#pragma omp barrier
      
#pragma omp for schedule(static, 1)
      for(unsigned t = 0; t < 16; t++){
	int p = 0;
	int tid_2 = omp_get_thread_num();
	std::cout << "There" << tid_2 <<std::endl;
	while(p < counter[tid_2]){
	  csketch.add(tid/2, buffer[tid_2][counter[p]], 1);
	  p++;
	}
      }
      
      wtimethread = omp_get_wtime() - cp2;
      //#pragma omp critical
        wtimesthread[tid] += wtimethread;
      }
    }
    times[nt] = omp_get_wtime() - t1;
    double mymax_h = htimesthread[0];
    double mymax_w = wtimesthread[0];
    for(int j = 1; j <= no_max_threads; j++){
      if (htimesthread[j] > mymax_h){
        mymax_h = htimesthread[j];
      }
      if (wtimesthread[j] > mymax_w){
        mymax_w = wtimesthread[j];
      }
    }
    htimestotal[nt] = mymax_h;
    wtimestotal[nt] = mymax_w;
    std::cout << "\tNo thr.: " << nt << "\ttime: " << times[nt] << "\thtime:" << htimestotal[nt] << "\twtime:" << wtimestotal[nt] << " " << "\tthrough.: " << no_stream / times[nt];
#ifdef CERROR
    std::cout << "\terr:" << csketch.getError(freq_decreasing, TOP, separate_hash) << "\ttop_item(id:" <<  freq_decreasing[0].second << "\treal:" << freq_decreasing[0].first << "\tquery:" << csketch.query(freq_decreasing[0].second,separate_hash) << ")";
#endif
    std::cout << std::endl;
    // cout << sum << endl;
      
  
  
  std::cout << std:: endl;
  delete [] times;
  }
  
  

void experiment_single_table_mbp16false(CSketch_MP<stype, dtype>& csketch, dtype* data, std::pair<stype, dtype>* &freq_decreasing) {
  double *times = new double[no_max_threads + 1];
  double *htimesthread = new double[no_max_threads + 1];
  double *htimestotal = new double[no_max_threads + 1];
  double *wtimesthread = new double[no_max_threads + 1];
  double *wtimestotal = new double[no_max_threads + 1];
  
  memset(times, 0, sizeof(double) * (no_max_threads +1));
  memset(htimestotal, 0, sizeof(double) * (no_max_threads +1));
  memset(wtimestotal, 0, sizeof(double) * (no_max_threads +1));
  
  const unsigned batch_size = 1024;
  unsigned no_batch = no_stream / batch_size;
  unsigned no_cols = csketch.get_no_cols();

  TabularHashMerged *separate_hash = new TabularHashMerged;
  uint32_t g_results_m[8 * batch_size * 16];  // 8*1024 elements for every thread  
  unsigned buffer[16][16384];
  unsigned int half = csketch.get_no_cols() / 2;

  unsigned counters[16];
  std::fill_n(&counters[0], 16, 0);

  unsigned nt = 16;
  
  csketch.reset();
  memset(htimesthread, 0, sizeof(double) * (no_max_threads +1));
  memset(wtimesthread, 0, sizeof(double) * (no_max_threads +1));
  double t1 = omp_get_wtime();

  //std::cout << "before batch" << std::endl;
  for(unsigned b = 0; b < no_batch/nt; b++) { // Partitions data[] by b
    
#pragma omp parallel num_threads(16) 
    {
      int tid = omp_get_thread_num();
      uint32_t* results_m = g_results_m + (tid * (8 * batch_size)); // claim 8192 bin for every thread
      double cp1, cp2, htimethread, wtimethread;
      
      cp1 = omp_get_wtime();
#pragma omp barrier
      
      unsigned long long start = b * (batch_size * 16) + (tid * batch_size);
      unsigned long long end = start + batch_size;
      
      for(unsigned long long i = start; i < end; i++) {
	separate_hash->hash(data[i], results_m);//, no_cols);
	//std::cout << "hashing" << std::endl;
	//#pragma omp for schedule(static, 1)
	//for(unsigned t = 0; t < 15; t++)
	//{
	if((*results_m % no_cols) < half){
	  buffer[0][counters[0]] = *results_m % no_cols;
	  counters[0] += 1;
	  //std::cout << "but where are you " << tid << std::endl;
	}
	  else{
	    buffer[1][counters[1]] = *results_m % no_cols;
	    counters[1] += 1;
	  }
	  results_m += 1;
	  if((*results_m % no_cols) < half){
	    buffer[2][counters[2]] = *results_m % no_cols;
	    counters[2] += 1;
	  }
	  else{
	    buffer[3][counters[3]] = *results_m % no_cols;
	    counters[3] += 1;
	  }
	  results_m += 1;
	  if((*results_m % no_cols) < half){
	    buffer[4][counters[4]] = *results_m % no_cols;
	    counters[4] += 1;
	  }
	  else{
	    buffer[5][counters[5]] = *results_m % no_cols;
	    counters[5] += 1;
	  }
	  results_m += 1;
	  if((*results_m % no_cols) < half){
	    buffer[6][counters[6]] = *results_m % no_cols;
	    counters[6] += 1;
	  }
	  else{
	    buffer[7][counters[7]] = *results_m % no_cols;
	    counters[7] += 1;
	  }
	  results_m += 1;
	  if((*results_m % no_cols) < half){
	    buffer[8][counters[8]] = *results_m % no_cols;
	    counters[8] += 1;
	  }
	  else{
	    buffer[9][counters[9]] = *results_m % no_cols;
	    counters[9] += 1;
	  }
	  results_m += 1;
	  if((*results_m % no_cols) < half){
	    buffer[10][counters[10]] = *results_m % no_cols;
	    counters[10] += 1;
	  }
	  else{
	    buffer[11][counters[11]] = *results_m % no_cols;
	    counters[11] += 1;
	  }
	  results_m += 1;
	  if((*results_m % no_cols) < half){
	    buffer[12][counters[12]] = *results_m % no_cols;
	    counters[12] += 1;
	  }
	  else{
	    buffer[13][counters[13]] = *results_m % no_cols;
	    counters[13] += 1;
	  }
	  results_m += 1;
	  if((*results_m % no_cols) < half){
	    buffer[14][counters[14]] = *results_m % no_cols;
	    counters[14] += 1;
	  }
	  else{
	    buffer[15][counters[15]] = *results_m % no_cols;
	    counters[15] += 1;
	  }
	  results_m += 1;
	  //}
	  //std::cout << "there1? " << tid << std::endl;
	  //results_m += 8;
	}
	htimethread = omp_get_wtime() - cp1;
	
	//#pragma omp critical
	htimesthread[tid] += htimethread;
	//}
      //#pragma omp parallel for schedule(static, 1) num_threads(nt) 

    #pragma omp barrier      	
  //std::cout << "before write" << std::endl;
	cp2 = omp_get_wtime();
#pragma omp for schedule(static, 1) 
      for(unsigned k = 0; k < 15; k++) {
        while(counters[tid] != (unsigned)-1){
	  //   std::cout << "here " << tid << " " << counters[tid] << std::endl;
          csketch.add(k/2, buffer[tid][counters[tid]], 1);
	  //std::cout << "there2? " << tid << " " << k << std::endl;
	  // std::cout << tid << ": " << counters[tid] << std::endl;
          counters[tid] -= 1;
	}
      }
      counters[tid] = 0;
      //std::cout << "BatchDone" << b <<std::endl;
      wtimethread = omp_get_wtime() - cp2;
      //#pragma omp critical

        wtimesthread[tid] += wtimethread;
      }
    }
    times[nt] = omp_get_wtime() - t1;
    double mymax_h = htimesthread[0];
    double mymax_w = wtimesthread[0];
    for(int j = 1; j <= no_max_threads; j++){
      if (htimesthread[j] > mymax_h){
        mymax_h = htimesthread[j];
      }
      if (wtimesthread[j] > mymax_w){
        mymax_w = wtimesthread[j];
      }
    }
    htimestotal[nt] = mymax_h;
    wtimestotal[nt] = mymax_w;
    std::cout << "\tNo thr.: " << nt << "\ttime: " << times[nt] << "\thtime:" << htimestotal[nt] << "\twtime:" << wtimestotal[nt] << " " << "\tthrough.: " << no_stream / times[nt];
#ifdef CERROR
    std::cout << "\terr:" << csketch.getError(freq_decreasing, TOP, separate_hash) << "\ttop_item(id:" <<  freq_decreasing[0].second << "\treal:" << freq_decreasing[0].first << "\tquery:" << csketch.query(freq_decreasing[0].second,separate_hash) << ")";
#endif
    std::cout << std::endl;
    // cout << sum << endl;
      
  //}
  
  std::cout << std:: endl;
  delete [] times;
  }

/*void experiment_single_table_mbpmod(CSketch_MP<stype, dtype>& csketch, dtype* data, std::pair<stype, dtype>* &freq_decreasing) {
  double *times = new double[no_max_threads + 1];
  memset(times, 0, sizeof(double) * (no_max_threads +1));

  const unsigned batch_size = 1024;
  unsigned no_batch = no_stream / batch_size;
  unsigned no_cols = csketch.get_no_cols();

  uint16_t modulos = no_cols;

  TabularHashMerged *separate_hash = new TabularHashMerged;
  uint16_t g_results_m[8 * batch_size * no_max_threads];    
  
  for(unsigned nt = 1; nt <= no_max_threads; nt *= 2) {
    csketch.reset();
    double t1 = omp_get_wtime();
    //int sum = 0;
    for(unsigned b = 0; b < no_batch/nt; b++) {
      
#pragma omp parallel num_threads(nt) 
      {
	int tid = omp_get_thread_num();
	uint16_t* results_m = g_results_m + (tid * (8 * batch_size));

	
	unsigned long long start = b * (batch_size * nt) + (tid * batch_size);
	unsigned long long end = start + batch_size;
	for(unsigned long long i = start; i < end; i++) {
	  separate_hash->modhash(data[i], results_m, modulos);// % no_cols);
	  results_m += 8;
	}
	//}

      //#pragma omp parallel for schedule(static, 1) num_threads(nt) 

	//#pragma omp barrier      	

	
#pragma omp for schedule(static, 1) //reduction(+:sum)
      for(unsigned k = 0; k < 8; k++) {                          
	for(unsigned i = 0; i < (batch_size * nt); i++) {                                                                                                              
	  unsigned col_id = (g_results_m[(i * 8) + k]);        
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
    std::cout << "\terr:" << csketch.getError(freq_decreasing, TOP, separate_hash) << "\ttop_item(id:" <<  freq_decreasing[0].second << "\treal:" << freq_decreasing[0].first << "\tquery:" << csketch.query(freq_decreasing[0].second,separate_hash) << ")";
#endif
    std::cout << std::endl;
    // cout << sum << endl;
      
  }
  
  std::cout << std:: endl;
  delete [] times;
  }

  void experiment_single_table_mbmod(CSketch_M<stype, dtype>& csketch, dtype* data, std::pair<stype, dtype>* &freq_decreasing) {
  double *times = new double[no_max_threads + 1];
  memset(times, 0, sizeof(double) * (no_max_threads +1));

  const unsigned batch_size = 1024;
  unsigned no_batch = no_stream / batch_size;
  unsigned no_cols = csketch.get_no_cols();

  uint16_t modulos = no_cols;

  TabularHashMerged *separate_hash = new TabularHashMerged;
  uint16_t g_results_m[8 * batch_size * no_max_threads];    
    
  for(unsigned nt = 1; nt <= no_max_threads; nt *= 2) {
    csketch.reset();
    double t1 = omp_get_wtime();

    for(unsigned b = 0; b < no_batch/nt; b++) {
#pragma omp parallel num_threads(nt) 
      {
	int tid = omp_get_thread_num();
	uint16_t* results_m = g_results_m + (tid * (8 * batch_size));
	
	unsigned long long start = b * (batch_size * nt) + (tid * batch_size);
	unsigned long long end = start + batch_size;

  for(unsigned long long i = start; i < end; i++) {

    separate_hash->modhash(data[i], results_m, modulos);
	  results_m += 8;
	}
	//}
      //#pragma omp parallel for schedule(static, 1) num_threads(nt) 
	
	//#pragma omp barrier      	
  
#pragma omp for schedule(static, 1)  
      for(unsigned k = 0; k < 8; k++) {                          
	for(unsigned i = 0; i < (batch_size * nt); i++) {                                                                                                              
	  unsigned col_id = (g_results_m[(i * 8) + k]);// % no_cols;        
	  csketch.add(k, col_id, 1); 
	}                                                                                                                                                          
      }
    }
    }
    times[nt] = omp_get_wtime() - t1;
    std::cout << "\tNo thr.: " << nt << "\ttime: " << times[nt] << "\tthrough.: " << no_stream / times[nt];
#ifdef CERROR
    std::cout << "\terr:" << csketch.getError(freq_decreasing, TOP, separate_hash) << "\ttop_item(id:" <<  freq_decreasing[0].second << "\treal:" << freq_decreasing[0].first << "\tquery:" << csketch.query(freq_decreasing[0].second,separate_hash) << ")";
#endif
    std::cout << std::endl;
  }
  
  std::cout << std:: endl;
  delete [] times;
}*/


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
TabularHashMerged *h_hash_merged;
  TabularHash *h_hashes;
  
  //h_hash_merged->setHashTables(h_hashes);//replaced h with k

  CountMinSketch_M<stype, dtype> cms_m(epsilon, delta);
  CountMinSketch_MP<stype, dtype> cms_mp(epsilon, delta);
  
  //CountMinSketch_MP<stype, dtype> cms_mp(epsilon, delta); 
  //cms_mp.set_hash(h_hash_merged);
  unsigned no_hashes = cms_m.get_no_rows();
  std::cout << "No of elements in stream : " << no_stream << std::endl;

  
  std::cout << "Size of the sketch: " << cms_m.get_no_rows()  << " by " << cms_m.get_no_cols() <<  std::endl;
 
  std::cout<<"Experimenting with single table " <<endl;
 
  std::cout<<"Experimenting with batch " <<endl;
  //experiment_single_table_mb(cms_m,data,freq_decreasing);
  experiment_single_table_mbp(cms_mp,data,freq_decreasing);
  //experiment_single_table_mbp16(cms_mp,data,freq_decreasing);
  //  experiment_single_table_mbp16false(cms_mp,data,freq_decreasing);
  experiment_single_table_mbp_2(cms_mp,data,freq_decreasing);
 
  /*********************************************************************************/
  delete [] data;
#ifdef CERROR
  delete [] freq_decreasing;
#endif
  return 0;
}
