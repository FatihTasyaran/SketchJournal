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
double zipfAlp;
int parseInputs(int argc, char** argv) {
  if(argc >= 6) {
    no_stream = pow(2, atoi(argv[1]));
    no_unique = pow(2, atoi(argv[2]));
    epsilon = atof(argv[3]);
    delta = atof(argv[4]);
    no_max_threads  = atoi(argv[5]);
    distNum=atoi(argv[6]);
    zipfAlp= atof(argv[7]);
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
  
  
  const unsigned batch_size = 1 * 1024;
  unsigned no_batch = no_stream / batch_size;
  unsigned no_cols = csketch.get_no_cols();
  
  TabularHashMerged *separate_hash = new TabularHashMerged;
  uint32_t g_results_m[8 * batch_size * no_max_threads];    

  double WHT, SHT;
  int fastBatch_hash, slowBatch_hash;
  double fast2slowRatio_hash;

  double WWT, SWT;
  int fastbatch_write, slowbatch_write;
  double fast2slowRatio_write;
  
  for(unsigned nt = 1; nt <= no_max_threads; nt *= 2) {
    
    int bnt = batch_size * nt;
    fast2slowRatio_hash = 1.0;
    fast2slowRatio_write = 1.0;
   
    csketch.reset();
    memset(htimesthread, 0, sizeof(double) * (no_max_threads +1));
    memset(wtimesthread, 0, sizeof(double) * (no_max_threads +1));
    double t1 = omp_get_wtime();
    for(unsigned b = 0; b < no_batch/nt; b++) {
      if(nt == 8) {
	WHT = 0;
	SHT = 0;
	fastBatch_hash = ((1 - 1.0f/(fast2slowRatio_hash + 1)) * bnt) / 4;
	slowBatch_hash = (2 * batch_size) - fastBatch_hash;
	//#pragma omp critical
	//std::cout << fastBatch_hash << " " << slowBatch_hash << std::endl;
      }
#pragma omp parallel num_threads(nt) 
      {
	int tid = omp_get_thread_num();
	int core_id = sched_getcpu();

	double cp1, cp2, htimethread, wtimethread;

	

	unsigned long long start = b * bnt + (tid * batch_size);
	unsigned long long end = start + batch_size;
	uint32_t* results_m = g_results_m + (tid * (8 * batch_size));
	
	if(nt == 8) {
	  if(tid >= 4) {
	    start = (b * bnt) + (core_id * slowBatch_hash);
	    end = start + slowBatch_hash;
	  } else {
	    start = (b * bnt) + (4 * slowBatch_hash) + ((core_id-4) * fastBatch_hash);
	    if(tid != 7) { 
	      end = start + fastBatch_hash;
	    } else {
	      end = ((b+1) * bnt);
	    }
	  }
	  //#pragma omp critical
	  //{
	  // std::cout << "b: " << b << " Thread: " << tid << " Core id: " << core_id << " start: " << start << std::endl;
	  //}
	  results_m = g_results_m + (8 * (start - (b * bnt)));
	}
	cp1 = omp_get_wtime();
	double stid = omp_get_wtime();
	for(unsigned long long i = start; i < end; i++) {
	  separate_hash->hash(data[i], results_m);
	  results_m += 8;
	}
	htimethread = omp_get_wtime() - cp1;
	double hash_time = omp_get_wtime() - stid;
	if(nt == 8) {
	  double tid = omp_get_thread_num();
	  if(tid >= 4) {
#pragma omp critical
	    {
	      if(hash_time > WHT) {
		WHT = hash_time;
	      }
	    }
	  } else {
#pragma omp critical
	    {
	      if(hash_time > SHT) {
		SHT = hash_time;
	      }
	    }
	  }
	}		
	
	/*if(nt == 8) {
	#pragma omp critical
	std::cout << tid << ": " << htimethread << std::endl;
	}*/
	htimesthread[tid] += htimethread;
  
	//}
	//#pragma omp parallel for schedule(static, 1) num_threads(nt) 
	
	//#pragma omp barrier      	
	
	cp2 = omp_get_wtime();
	if(nt != 8) {
	  
#pragma omp for schedule(static, 1)  
	  for(unsigned k = 0; k < 8; k++) {                          
	    for(unsigned i = 0; i < (batch_size * nt); i++) {                                                                                                              
	      unsigned col_id = (g_results_m[(i * 8) + k]) % no_cols;        
	      csketch.add(k, col_id, 1); 
	    }                                                                                                                                                          
	  }
	} else {
	  WWT = SWT = 0;
	  int row_id, row_id_2;
	  double fast2slowRatio = fast2slowRatio_write;
	  int fastBatch = (1 - (1.0f/(fast2slowRatio + 1))) * bnt;
	  int slowBatch = bnt - fastBatch;
	  int wbatch_size = 0, obatch_size = 0;
	  //#pragma omp single
	  //{
	  //std::cout << fastBatch << " " << slowBatch << std::endl;
	  //}
	  if(tid < 4) {
	    row_id =tid;
	    row_id_2 = tid + 4;
	    wbatch_size = fastBatch;
	    obatch_size = slowBatch;
	  } else {
	    row_id = tid;
	    row_id_2 = tid - 4;
	    wbatch_size = slowBatch;
	    obatch_size = fastBatch;
	  }
	  double wtid = omp_get_wtime();
	  for (unsigned i = 0; i < wbatch_size; i++) {
	    unsigned col_id = (g_results_m[(i * 8) + row_id]) % no_cols;
	    csketch.add(row_id, col_id, 1);
	  }
	  //double write_time = omp_get_wtime() - wtid;
	  //#pragma omp barrier
	  for (unsigned i = obatch_size; i < bnt; i++) {
	    unsigned col_id_2 = (g_results_m[(i * 8) + row_id_2]) % no_cols;
	    csketch.add(row_id_2, col_id_2, 1);
	  }
	  double write_time = omp_get_wtime() - wtid;
	  
	  if(tid >= 4) {
#pragma omp critical
	    {
	      if(write_time > WWT){
		WWT = write_time; 
	      }  
	    }
	  } else {
#pragma omp critical
	  {
	    if(write_time > SWT) {
	      SWT = write_time;
	    }
	  }
	  }
	  
	  if(nt == 8) {
	    double SHTp = SHT / fastBatch_hash;
	    double WHTp = WHT / slowBatch_hash;
	    if(SHT - WHT > 0) {
	      double x = (SHT - WHT) / (SHTp + WHTp);
	      fast2slowRatio_hash = (fastBatch_hash - x) / (slowBatch_hash + x);
	    } else if (SHT - WHT < 0){
	      double x = (WHT - SHT) / (SHTp + WHTp);
	      fast2slowRatio_hash = (fastBatch_hash + x) / (slowBatch_hash - x);
	    }
	    if(fast2slowRatio_hash < 1) fast2slowRatio_hash = 1;

	    double SWTp = SWT / fastBatch;// + slowBatch;
	    double WWTp = WWT / slowBatch;// + fastBatch;
	    if(SWT - WWT > 0) {
	      double y = (SWT - WWT) / (SWTp + WWTp);
	      fast2slowRatio_write = (fastBatch - y) / (slowBatch + y);
	    } else if (SWT - WWT < 0){
	      double y = (WWT - SWT) / (SWTp + WWTp);
	      fast2slowRatio_write = (fastBatch + y) / (slowBatch - y);
	    }
	    //fast2slowRatio_write = 1.3;
	    if(fast2slowRatio_write < 1) fast2slowRatio_write = 1;
	    #pragma omp single
	    {
	      double fasthash = fastBatch_hash;
	      double slowhash = slowBatch_hash;
	      double fastwrite = fastBatch;
	      double slowwrite = slowBatch;
	      //std::cout << (fasthash/slowhash) << "," << (fastwrite/slowwrite) << std::endl;
	      //std::cout << fast2slowRatio_hash << "," << fast2slowRatio_write << std::endl;
	    }
	    //#pragma omp critical
	    //{
	      //std::cout << "SWTp: " << SWTp << " WWTp: " << WWTp << " SWT: " << SWT <<" WWT: " << WWT << " " << fast2slowRatio_write << " " << fastBatch << " " << slowBatch << std::endl;
	    //}
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
    std::cout << "no thr:. " << nt << "\ttime: " << times[nt] << "\thtime:" << htimestotal[nt] << "\twtime:" << wtimestotal[nt] << " " << "\tthrough.: " << no_stream / times[nt];
    //#ifdef CERROR
      std::cout << "\terror:" << csketch.getError(freq_decreasing, TOP, separate_hash) << "\ttop_item(id:" <<  freq_decreasing[0].second << "\treal:" << freq_decreasing[0].first << "\tquery:" << csketch.query(freq_decreasing[0].second,separate_hash) << ")";
    //#endif
    std::cout << std::endl;
  }
    
  
  std::cout << std:: endl;
    delete [] times;
}


void experiment_single_table_mb_half_barrier(CSketch_M<stype, dtype>& csketch, dtype* data, std::pair<stype, dtype>* &freq_decreasing) {
  double *times = new double[no_max_threads + 1];
  double *htimesthread = new double[no_max_threads + 1];
  double *htimestotal = new double[no_max_threads + 1];
  double *wtimesthread = new double[no_max_threads + 1];
  double *wtimestotal = new double[no_max_threads + 1];

  memset(times, 0, sizeof(double) * (no_max_threads +1));
  memset(htimestotal, 0, sizeof(double) * (no_max_threads +1));
  memset(wtimestotal, 0, sizeof(double) * (no_max_threads +1));
  
  
  const unsigned batch_size = 1 * 1024;
  unsigned no_batch = no_stream / batch_size;
  unsigned no_cols = csketch.get_no_cols();
  
  TabularHashMerged *separate_hash = new TabularHashMerged;
  uint32_t g_results_m[8 * batch_size * no_max_threads];    

  double WHT, SHT;
  int fastBatch_hash, slowBatch_hash;
  double fast2slowRatio_hash;

  double WWT, SWT;
  int fastbatch_write, slowbatch_write;
  double fast2slowRatio_write;
  
  for(unsigned nt = 1; nt <= no_max_threads; nt *= 2) {
    
    int bnt = batch_size * nt;
    fast2slowRatio_hash = 1.0;
    fast2slowRatio_write = 1.0;
   
    csketch.reset();
    memset(htimesthread, 0, sizeof(double) * (no_max_threads +1));
    memset(wtimesthread, 0, sizeof(double) * (no_max_threads +1));
    double t1 = omp_get_wtime();
    for(unsigned b = 0; b < no_batch/nt; b++) {
      if(nt == 8) {
	WHT = 0;
	SHT = 0;
	fastBatch_hash = ((1 - 1.0f/(fast2slowRatio_hash + 1)) * bnt) / 4;
	slowBatch_hash = (2 * batch_size) - fastBatch_hash;
	//#pragma omp critical
	//std::cout << fastBatch_hash << " " << slowBatch_hash << std::endl;
      }
#pragma omp parallel num_threads(nt) 
      {
	int tid = omp_get_thread_num();
	int core_id = sched_getcpu();

	double cp1, cp2, htimethread, wtimethread;

	

	unsigned long long start = b * bnt + (tid * batch_size);
	unsigned long long end = start + batch_size;
	uint32_t* results_m = g_results_m + (tid * (8 * batch_size));
	
	if(nt == 8) {
	  if(tid >= 4) {
	    start = (b * bnt) + (core_id * slowBatch_hash);
	    end = start + slowBatch_hash;
	  } else {
	    start = (b * bnt) + (4 * slowBatch_hash) + ((core_id-4) * fastBatch_hash);
	    if(tid != 7) { 
	      end = start + fastBatch_hash;
	    } else {
	      end = ((b+1) * bnt);
	    }
	  }
	  //#pragma omp critical
	  //{
	  // std::cout << "b: " << b << " Thread: " << tid << " Core id: " << core_id << " start: " << start << std::endl;
	  //}
	  results_m = g_results_m + (8 * (start - (b * bnt)));
	}
	cp1 = omp_get_wtime();
	double stid = omp_get_wtime();
	for(unsigned long long i = start; i < end; i++) {
	  separate_hash->hash(data[i], results_m);
	  results_m += 8;
	}
	htimethread = omp_get_wtime() - cp1;
	double hash_time = omp_get_wtime() - stid;
	if(nt == 8) {
	  double tid = omp_get_thread_num();
	  if(tid >= 4) {
#pragma omp critical
	    {
	      if(hash_time > WHT) {
		WHT = hash_time;
	      }
	    }
	  } else {
#pragma omp critical
	    {
	      if(hash_time > SHT) {
		SHT = hash_time;
	      }
	    }
	  }
	}		
	
	/*if(nt == 8) {
	#pragma omp critical
	std::cout << tid << ": " << htimethread << std::endl;
	}*/
	htimesthread[tid] += htimethread;
  
	//}
	//#pragma omp parallel for schedule(static, 1) num_threads(nt) 
	
	#pragma omp barrier      	
	
	cp2 = omp_get_wtime();
	if(nt != 8) {
	  
#pragma omp for schedule(static, 1)  
	  for(unsigned k = 0; k < 8; k++) {                          
	    for(unsigned i = 0; i < (batch_size * nt); i++) {                                                                                                              
	      unsigned col_id = (g_results_m[(i * 8) + k]) % no_cols;        
	      csketch.add(k, col_id, 1); 
	    }                                                                                                                                                          
	  }
	} else {
	  WWT = SWT = 0;
	  int row_id, row_id_2;
	  double fast2slowRatio = fast2slowRatio_write;
	  int fastBatch = (1 - (1.0f/(fast2slowRatio + 1))) * bnt;
	  int slowBatch = bnt - fastBatch;
	  int wbatch_size = 0, obatch_size = 0;
	  //#pragma omp single
	  //{
	  //std::cout << fastBatch << " " << slowBatch << std::endl;
	  //}
	  if(tid < 4) {
	    row_id =tid;
	    row_id_2 = tid + 4;
	    wbatch_size = fastBatch;
	    obatch_size = slowBatch;
	  } else {
	    row_id = tid;
	    row_id_2 = tid - 4;
	    wbatch_size = slowBatch;
	    obatch_size = fastBatch;
	  }
	  double wtid = omp_get_wtime();
	  for (unsigned i = 0; i < wbatch_size; i++) {
	    unsigned col_id = (g_results_m[(i * 8) + row_id]) % no_cols;
	    csketch.add(row_id, col_id, 1);
	  }
	  //double write_time = omp_get_wtime() - wtid;
	  //#pragma omp barrier
	  for (unsigned i = obatch_size; i < bnt; i++) {
	    unsigned col_id_2 = (g_results_m[(i * 8) + row_id_2]) % no_cols;
	    csketch.add(row_id_2, col_id_2, 1);
	  }
	  double write_time = omp_get_wtime() - wtid;
	  
	  if(tid >= 4) {
#pragma omp critical
	    {
	      if(write_time > WWT){
		WWT = write_time; 
	      }  
	    }
	  } else {
#pragma omp critical
	  {
	    if(write_time > SWT) {
	      SWT = write_time;
	    }
	  }
	  }
	  
	  if(nt == 8) {
	    double SHTp = SHT / fastBatch_hash;
	    double WHTp = WHT / slowBatch_hash;
	    if(SHT - WHT > 0) {
	      double x = (SHT - WHT) / (SHTp + WHTp);
	      fast2slowRatio_hash = (fastBatch_hash - x) / (slowBatch_hash + x);
	    } else if (SHT - WHT < 0){
	      double x = (WHT - SHT) / (SHTp + WHTp);
	      fast2slowRatio_hash = (fastBatch_hash + x) / (slowBatch_hash - x);
	    }
	    if(fast2slowRatio_hash < 1) fast2slowRatio_hash = 1;

	    double SWTp = SWT / fastBatch;// + slowBatch;
	    double WWTp = WWT / slowBatch;// + fastBatch;
	    if(SWT - WWT > 0) {
	      double y = (SWT - WWT) / (SWTp + WWTp);
	      fast2slowRatio_write = (fastBatch - y) / (slowBatch + y);
	    } else if (SWT - WWT < 0){
	      double y = (WWT - SWT) / (SWTp + WWTp);
	      fast2slowRatio_write = (fastBatch + y) / (slowBatch - y);
	    }
	    //fast2slowRatio_write = 1.3;
	    if(fast2slowRatio_write < 1) fast2slowRatio_write = 1;
	    #pragma omp single
	    {
	      double fasthash = fastBatch_hash;
	      double slowhash = slowBatch_hash;
	      double fastwrite = fastBatch;
	      double slowwrite = slowBatch;
	      //std::cout << (fasthash/slowhash) << "," << (fastwrite/slowwrite) << std::endl;
	      //std::cout << fast2slowRatio_hash << "," << fast2slowRatio_write << std::endl;
	    }
	    //#pragma omp critical
	    //{
	      //std::cout << "SWTp: " << SWTp << " WWTp: " << WWTp << " SWT: " << SWT <<" WWT: " << WWT << " " << fast2slowRatio_write << " " << fastBatch << " " << slowBatch << std::endl;
	    //}
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
    std::cout << "no thr:. " << nt << "\ttime: " << times[nt] << "\thtime:" << htimestotal[nt] << "\twtime:" << wtimestotal[nt] << " " << "\tthrough.: " << no_stream / times[nt];
    //#ifdef CERROR
      std::cout << "\terror:" << csketch.getError(freq_decreasing, TOP, separate_hash) << "\ttop_item(id:" <<  freq_decreasing[0].second << "\treal:" << freq_decreasing[0].first << "\tquery:" << csketch.query(freq_decreasing[0].second,separate_hash) << ")";
    //#endif
    std::cout << std::endl;
  }
    
  
  std::cout << std:: endl;
    delete [] times;
}

void experiment_single_table_mb_full_barrier(CSketch_M<stype, dtype>& csketch, dtype* data, std::pair<stype, dtype>* &freq_decreasing) {
  double *times = new double[no_max_threads + 1];
  double *htimesthread = new double[no_max_threads + 1];
  double *htimestotal = new double[no_max_threads + 1];
  double *wtimesthread = new double[no_max_threads + 1];
  double *wtimestotal = new double[no_max_threads + 1];

  memset(times, 0, sizeof(double) * (no_max_threads +1));
  memset(htimestotal, 0, sizeof(double) * (no_max_threads +1));
  memset(wtimestotal, 0, sizeof(double) * (no_max_threads +1));
  
  
  const unsigned batch_size = 1 * 1024;
  unsigned no_batch = no_stream / batch_size;
  unsigned no_cols = csketch.get_no_cols();
  
  TabularHashMerged *separate_hash = new TabularHashMerged;
  uint32_t g_results_m[8 * batch_size * no_max_threads];    

  double WHT, SHT;
  int fastBatch_hash, slowBatch_hash;
  double fast2slowRatio_hash;

  double WWT, SWT;
  int fastbatch_write, slowbatch_write;
  double fast2slowRatio_write;
  
  for(unsigned nt = 1; nt <= no_max_threads; nt *= 2) {
    
    int bnt = batch_size * nt;
    fast2slowRatio_hash = 1.0;
    fast2slowRatio_write = 1.0;
   
    csketch.reset();
    memset(htimesthread, 0, sizeof(double) * (no_max_threads +1));
    memset(wtimesthread, 0, sizeof(double) * (no_max_threads +1));
    double t1 = omp_get_wtime();
    for(unsigned b = 0; b < no_batch/nt; b++) {
      if(nt == 8) {
	WHT = 0;
	SHT = 0;
	fastBatch_hash = ((1 - 1.0f/(fast2slowRatio_hash + 1)) * bnt) / 4;
	slowBatch_hash = (2 * batch_size) - fastBatch_hash;
	//#pragma omp critical
	//std::cout << fastBatch_hash << " " << slowBatch_hash << std::endl;
      }
#pragma omp parallel num_threads(nt) 
      {
	int tid = omp_get_thread_num();
	int core_id = sched_getcpu();

	double cp1, cp2, htimethread, wtimethread;

	

	unsigned long long start = b * bnt + (tid * batch_size);
	unsigned long long end = start + batch_size;
	uint32_t* results_m = g_results_m + (tid * (8 * batch_size));
	
	if(nt == 8) {
	  if(tid >= 4) {
	    start = (b * bnt) + (core_id * slowBatch_hash);
	    end = start + slowBatch_hash;
	  } else {
	    start = (b * bnt) + (4 * slowBatch_hash) + ((core_id-4) * fastBatch_hash);
	    if(tid != 7) { 
	      end = start + fastBatch_hash;
	    } else {
	      end = ((b+1) * bnt);
	    }
	  }
	  //#pragma omp critical
	  //{
	  // std::cout << "b: " << b << " Thread: " << tid << " Core id: " << core_id << " start: " << start << std::endl;
	  //}
	  results_m = g_results_m + (8 * (start - (b * bnt)));
	}
	cp1 = omp_get_wtime();
	double stid = omp_get_wtime();
	for(unsigned long long i = start; i < end; i++) {
	  separate_hash->hash(data[i], results_m);
	  results_m += 8;
	}
	htimethread = omp_get_wtime() - cp1;
	double hash_time = omp_get_wtime() - stid;
	if(nt == 8) {
	  double tid = omp_get_thread_num();
	  if(tid >= 4) {
#pragma omp critical
	    {
	      if(hash_time > WHT) {
		WHT = hash_time;
	      }
	    }
	  } else {
#pragma omp critical
	    {
	      if(hash_time > SHT) {
		SHT = hash_time;
	      }
	    }
	  }
	}		
	
	/*if(nt == 8) {
	#pragma omp critical
	std::cout << tid << ": " << htimethread << std::endl;
	}*/
	htimesthread[tid] += htimethread;
  
	//}
	//#pragma omp parallel for schedule(static, 1) num_threads(nt) 

	#pragma omp barrier      	
	
	cp2 = omp_get_wtime();
	if(nt != 8) {
	  
#pragma omp for schedule(static, 1)  
	  for(unsigned k = 0; k < 8; k++) {                          
	    for(unsigned i = 0; i < (batch_size * nt); i++) {                                                                                                              
	      unsigned col_id = (g_results_m[(i * 8) + k]) % no_cols;        
	      csketch.add(k, col_id, 1); 
	    }                                                                                                                                                          
	  }
	} else {
	  WWT = SWT = 0;
	  int row_id, row_id_2;
	  double fast2slowRatio = fast2slowRatio_write;
	  int fastBatch = (1 - (1.0f/(fast2slowRatio + 1))) * bnt;
	  int slowBatch = bnt - fastBatch;
	  int wbatch_size = 0, obatch_size = 0;
	  //#pragma omp single
	  //{
	  //std::cout << fastBatch << " " << slowBatch << std::endl;
	  //}
	  if(tid < 4) {
	    row_id =tid;
	    row_id_2 = tid + 4;
	    wbatch_size = fastBatch;
	    obatch_size = slowBatch;
	  } else {
	    row_id = tid;
	    row_id_2 = tid - 4;
	    wbatch_size = slowBatch;
	    obatch_size = fastBatch;
	  }
	  double wtid = omp_get_wtime();
	  for (unsigned i = 0; i < wbatch_size; i++) {
	    unsigned col_id = (g_results_m[(i * 8) + row_id]) % no_cols;
	    csketch.add(row_id, col_id, 1);
	  }
	  //double write_time = omp_get_wtime() - wtid;
	  #pragma omp barrier
	  for (unsigned i = obatch_size; i < bnt; i++) {
	    unsigned col_id_2 = (g_results_m[(i * 8) + row_id_2]) % no_cols;
	    csketch.add(row_id_2, col_id_2, 1);
	  }
	  double write_time = omp_get_wtime() - wtid;
	  
	  if(tid >= 4) {
#pragma omp critical
	    {
	      if(write_time > WWT){
		WWT = write_time; 
	      }  
	    }
	  } else {
#pragma omp critical
	  {
	    if(write_time > SWT) {
	      SWT = write_time;
	    }
	  }
	  }
	  
	  if(nt == 8) {
	    double SHTp = SHT / fastBatch_hash;
	    double WHTp = WHT / slowBatch_hash;
	    if(SHT - WHT > 0) {
	      double x = (SHT - WHT) / (SHTp + WHTp);
	      fast2slowRatio_hash = (fastBatch_hash - x) / (slowBatch_hash + x);
	    } else if (SHT - WHT < 0){
	      double x = (WHT - SHT) / (SHTp + WHTp);
	      fast2slowRatio_hash = (fastBatch_hash + x) / (slowBatch_hash - x);
	    }
	    if(fast2slowRatio_hash < 1) fast2slowRatio_hash = 1;

	    double SWTp = SWT / fastBatch;// + slowBatch;
	    double WWTp = WWT / slowBatch;// + fastBatch;
	    if(SWT - WWT > 0) {
	      double y = (SWT - WWT) / (SWTp + WWTp);
	      fast2slowRatio_write = (fastBatch - y) / (slowBatch + y);
	    } else if (SWT - WWT < 0){
	      double y = (WWT - SWT) / (SWTp + WWTp);
	      fast2slowRatio_write = (fastBatch + y) / (slowBatch - y);
	    }
	    //fast2slowRatio_write = 1.3;
	    if(fast2slowRatio_write < 1) fast2slowRatio_write = 1;
	    #pragma omp single
	    {
	      double fasthash = fastBatch_hash;
	      double slowhash = slowBatch_hash;
	      double fastwrite = fastBatch;
	      double slowwrite = slowBatch;
	      //std::cout << (fasthash/slowhash) << "," << (fastwrite/slowwrite) << std::endl;
	      //std::cout << fast2slowRatio_hash << "," << fast2slowRatio_write << std::endl;
	    }
	    //#pragma omp critical
	    //{
	      //std::cout << "SWTp: " << SWTp << " WWTp: " << WWTp << " SWT: " << SWT <<" WWT: " << WWT << " " << fast2slowRatio_write << " " << fastBatch << " " << slowBatch << std::endl;
	    //}
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
    std::cout << "no thr:. " << nt << "\ttime: " << times[nt] << "\thtime:" << htimestotal[nt] << "\twtime:" << wtimestotal[nt] << " " << "\tthrough.: " << no_stream / times[nt];
    //#ifdef CERROR
      std::cout << "\terror:" << csketch.getError(freq_decreasing, TOP, separate_hash) << "\ttop_item(id:" <<  freq_decreasing[0].second << "\treal:" << freq_decreasing[0].first << "\tquery:" << csketch.query(freq_decreasing[0].second,separate_hash) << ")";
    //#endif
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

  double WHT, SHT;
  int fastBatch_hash, slowBatch_hash;
  double fast2slowRatio_hash;

  double WWT, SWT;
  int fastbatch_write, slowbatch_write;
  double fast2slowRatio_write;
  
  for(unsigned nt = 1; nt <= no_max_threads; nt *= 2) {

    int bnt = batch_size * nt;
    fast2slowRatio_hash = 1.0;
    fast2slowRatio_write = 1.0;
   
    csketch.reset();
    memset(htimesthread, 0, sizeof(double) * (no_max_threads +1));
    memset(wtimesthread, 0, sizeof(double) * (no_max_threads +1));
    double t1 = omp_get_wtime();
    for(unsigned b = 0; b < no_batch/nt; b++) {
      if(nt == 8) {
	WHT = 0;
	SHT = 0;
	fastBatch_hash = ((1 - 1.0f/(fast2slowRatio_hash + 1)) * bnt) / 4;
	slowBatch_hash = (2 * batch_size) - fastBatch_hash;
	//#pragma omp critical
	//std::cout << fastBatch_hash << " " << slowBatch_hash << std::endl;
      }
#pragma omp parallel num_threads(nt) 
      {
	int tid = omp_get_thread_num();
	int core_id = sched_getcpu();

	double cp1, cp2, htimethread, wtimethread;

	cp1 = omp_get_wtime();

	unsigned long long start = b * bnt + (tid * batch_size);
	unsigned long long end = start + batch_size;
	uint32_t* results_m = g_results_m + (tid * (8 * batch_size));
	
	if(nt == 8) {
	  if(tid >= 4) {
	    start = (b * bnt) + (core_id * slowBatch_hash);
	    end = start + slowBatch_hash;
	  } else {
	    start = (b * bnt) + (4 * slowBatch_hash) + ((core_id-4) * fastBatch_hash);
	    if(tid != 7) { 
	      end = start + fastBatch_hash;
	    } else {
	      end = ((b+1) * bnt);
	    }
	  }
	  //#pragma omp critical
	  //{
	  // std::cout << "b: " << b << " Thread: " << tid << " Core id: " << core_id << " start: " << start << std::endl;
	  //}
	  results_m = g_results_m + (8 * (start - (b * bnt)));
	}
	
	double stid = omp_get_wtime();
	for(unsigned long long i = start; i < end; i++) {
	  separate_hash->hash(data[i], results_m);
	  results_m += 8;
	}
	double hash_time = omp_get_wtime() - stid;
	if(nt == 8) {
	  double tid = omp_get_thread_num();
	  if(tid >= 4) {
#pragma omp critical
	    {
	      if(hash_time > WHT) {
		WHT = hash_time;
	      }
	    }
	  } else {
#pragma omp critical
	    {
	      if(hash_time > SHT) {
		SHT = hash_time;
	      }
	    }
	  }
	}		
	htimethread = omp_get_wtime() - cp1;
	/*if(nt == 8) {
	#pragma omp critical
	std::cout << tid << ": " << htimethread << std::endl;
	}*/
	htimesthread[tid] += htimethread;
  
	//}
	//#pragma omp parallel for schedule(static, 1) num_threads(nt) 
	
	//#pragma omp barrier      	
	
	cp2 = omp_get_wtime();
	if(nt != 8) {
	  
#pragma omp for schedule(static, 1)  
	  for(unsigned k = 0; k < 8; k++) {                          
	    for(unsigned i = 0; i < (batch_size * nt); i++) {                                                                                                              
	      unsigned col_id = (g_results_m[(i * 8) + k]) % no_cols;        
	      csketch.add(k, col_id, 1); 
	    }                                                                                                                                                          
	  }
	} else {
	  WWT = SWT = 0;
	  int row_id, row_id_2;
	  double fast2slowRatio = fast2slowRatio_write;
	  int fastBatch = (1 - (1.0f/(fast2slowRatio + 1))) * bnt;
	  int slowBatch = bnt - fastBatch;
	  int wbatch_size = 0, obatch_size = 0;
	  //std::cout << fastBatch << " " << slowBatch << std::endl;
	  if(tid < 4) {
	    row_id =tid;
	    row_id_2 = tid + 4;
	    wbatch_size = fastBatch;
	    obatch_size = slowBatch;
	  } else {
	    row_id = tid;
	    row_id_2 = tid - 4;
	    wbatch_size = slowBatch;
	    obatch_size = fastBatch;
	  }
	  double wtid = omp_get_wtime();
	  for (unsigned i = 0; i < wbatch_size; i++) {
	    unsigned col_id = (g_results_m[(i * 8) + row_id]) % no_cols;
	    csketch.add(row_id, col_id, 1);
	  }
	  //double write_time = omp_get_wtime() - wtid;
	  //#pragma omp barrier
	  for (unsigned i = obatch_size; i < bnt; i++) {
	    unsigned col_id_2 = (g_results_m[(i * 8) + row_id_2]) % no_cols;
	    csketch.add(row_id_2, col_id_2, 1);
	  }
	  double write_time = omp_get_wtime() - wtid;
	  
	  if(tid >= 4) {
#pragma omp critical
	    {
	      if(write_time > WWT){
		WWT = write_time; 
	      }  
	    }
	  } else {
#pragma omp critical
	  {
	    if(write_time > SWT) {
	      SWT = write_time;
	    }
	  }
	  }
	  
	  if(nt == 8) {
	    double SHTp = SHT / fastBatch_hash;
	    double WHTp = WHT / slowBatch_hash;
	    if(SHT - WHT > 0) {
	      double x = (SHT - WHT) / (SHTp + WHTp);
	      fast2slowRatio_hash = (fastBatch_hash - x) / (slowBatch_hash + x);
	    } else if (SHT - WHT < 0){
	      double x = (WHT - SHT) / (SHTp + WHTp);
	      fast2slowRatio_hash = (fastBatch_hash + x) / (slowBatch_hash - x);
	    }
	    if(fast2slowRatio_hash < 1) fast2slowRatio_hash = 1;

	    double SWTp = SWT / fastBatch;// + slowBatch;
	    double WWTp = WWT / slowBatch;// + fastBatch;
	    if(SWT - WWT > 0) {
	      double y = (SWT - WWT) / (SWTp + WWTp);
	      fast2slowRatio_write = (fastBatch - y) / (slowBatch + y);
	    } else if (SWT - WWT < 0){
	      double y = (WWT - SWT) / (SWTp + WWTp);
	      fast2slowRatio_write = (fastBatch + y) / (slowBatch - y);
	    }
	    //fast2slowRatio_write = 1.3;
	    if(fast2slowRatio_write < 1) fast2slowRatio_write = 1;
	    //std::cout << fast2slowRatio_write << std::endl;
	    //#pragma omp critical 
	    //std::cout << "SWTp: " << SWTp << " WWTp: " << WWTp << " SWT: " << SWT <<" WWT: " << WWT << " " << fast2slowRatio_write << " " << fastBatch << " " << slowBatch << std::endl;
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
    std::cout << "no thr:. " << nt << "\ttime:" << times[nt] << "\thtime:" << htimestotal[nt] << "\twtime:" << wtimestotal[nt] << " " << "\tthrough.: " << no_stream / times[nt];
#ifdef CERROR
      std::cout << "\terror:" << csketch.getError(freq_decreasing, TOP, separate_hash) << "\ttop_item(id:" <<  freq_decreasing[0].second << "\treal:" << freq_decreasing[0].first << "\tquery:" << csketch.query(freq_decreasing[0].second,separate_hash) << ")";
#endif
      std::cout << std::endl;
  }
    
  
    std::cout << std:: endl;
    delete [] times;
}


/*void experiment_single_table_mbp(CSketch_MP<stype, dtype>& csketch, dtype* data, std::pair<stype, dtype>* &freq_decreasing) {
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
  }*/


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
  TabularHashMerged *h_hash_merged;
  TabularHash *h_hashes;
  
  //h_hash_merged->setHashTables(h_hashes);//replaced h with k

  CountMinSketch_M<stype, dtype> cms_m(epsilon, delta);
  CountMinSketch_MP<stype, dtype> cms_mp(epsilon, delta);
  
  //CountMinSketch_MP<stype, dtype> cms_mp(epsilon, delta); 
  //cms_mp.set_hash(h_hash_merged);
  
  unsigned no_hashes = cms_m.get_no_rows();
  //std::cout << "No of elements in stream : " << no_stream << std::endl;
  //std::cout << "Size of the sketch: " << cms_m.get_no_rows()  << " by " << cms_m.get_no_cols() <<  std::endl;
  //std::cout<<"Experimenting with single table " <<endl;
 
  //std::cout<<"Experimenting with batch " <<endl;
  //std::cout << "No Affinity" << std::endl;
  //experiment_single_table_m(cms_m,data,freq_decreasing);
  
  std::cout << "No barrier" << std::endl;
  experiment_single_table_mb(cms_m,data,freq_decreasing);

  std::cout << "Half barrier" << std::endl;
  experiment_single_table_mb_half_barrier(cms_m,data,freq_decreasing);

  std::cout << "Full barrier" << std::endl;
  experiment_single_table_mb(cms_m,data,freq_decreasing);



  //std::cout << "Experimenting with batch and pdading" << std::endl;
  //experiment_single_table_mbp(cms_mp,data,freq_decreasing);
  
  
  delete [] data;
#ifdef CERROR
  delete [] freq_decreasing;
#endif
  return 0;
}
