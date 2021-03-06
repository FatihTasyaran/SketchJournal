#include <iostream>
#include <random>
#include <algorithm>
#include <vector>
#include <string.h>
#include <chrono>

#include "common.h"
//#include "_csketch.hpp"
#include "csketch_merged.hpp"
#include "hasher.hpp"
#include "hasher_merged.hpp"
#include "generator.h"
using sn = chrono::nanoseconds;
using get_time = chrono::steady_clock;
//#include "omp.h"

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
void experiment_single_table_m(CSketch_M<stype, dtype>& csketch, dtype* data, std::pair<stype, dtype>* &freq_decreasing) {
 
  
      csketch.reset();
  
      uint32_t results_m[256];
      for(int i =0; i< 256;i++)
	results_m[i]=0;
      cout<<"Starting"<<endl;
      auto start =  get_time::now();
      for(unsigned long long i = 0; i < no_stream; i++) {
        csketch.insert(data[i], results_m);
      }
      auto end =get_time::now();
      cout<<"Done"<<endl;

      auto diff = end-start;
      
      std::cout << "\tNo thr.: " << 1 << "\ttime: " <<  chrono::duration_cast<sn>(diff).count()*0.000000001 << "\tthrough.: " <<no_stream /(0.000000001* chrono::duration_cast<sn>(diff).count());
#ifdef CERROR
        std::cout << "\terr:" << csketch.getError(freq_decreasing, TOP) << "\ttop_item(id:" <<  freq_decreasing[0].second << "\treal:" << freq_decreasing[0].first << "\tquery:" << csketch.query(freq_decreasing[0].second) << ")";
#endif
    std::cout<< std::endl;
  
  std::cout<< std::endl;

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
TabularHashMerged *h_hash_merged = new TabularHashMerged;
  TabularHash *h_hashes;
  


  CountMinSketch_M<stype, dtype> cms_m(epsilon, delta);
  cms_m.set_hash(h_hash_merged);
  //  h_hash_merged->setHashTables(h_hashes);//replaced h with k
  unsigned no_hashes = cms_m.get_no_rows();
  std::cout << "No of elements in stream : " << no_stream << std::endl;

  
  std::cout << "Size of the sketch: " << cms_m.get_no_rows()  << " by " << cms_m.get_no_cols() <<  std::endl;
 
  std::cout<<"Experimenting with single table " <<endl;
 
  experiment_single_table_m(cms_m,data,freq_decreasing);



  
  
 
  /*********************************************************************************/
  delete [] data;
#ifdef CERROR
  delete [] freq_decreasing;
#endif
  return 0;
}
