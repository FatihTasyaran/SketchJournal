#ifndef CSKETCHMP_H
#define CSKETCHMP_H

#include <iostream>
#include <algorithm>
#include "hasher_merged.hpp"
#include "common.h"
#include "omp.h"
#include <fstream>
#include <string>
template <class s_type, class d_type>
class CSketch_MP {
  
protected:
  unsigned no_rows;
  unsigned no_cols;
  unsigned PSIZE;
  s_type* table;

  CSketch_MP(double epsilon, double delta) {
    no_rows = log2(1 / delta);
    no_cols = (2 / epsilon);

    //prime column count
    bool found = false;
    while(!found) {
      found = true;
      for(unsigned i = 2; i <= sqrt(no_cols); i++) {
	if(no_cols % i == 0) {
	  found = false;
	  no_cols++;
	}
      }
    }
    
    PSIZE = no_cols + 256;
    table = new s_type[this->no_rows * PSIZE];
    /*
    D(
    std::cout << "A sketch is created with " << this->no_rows << " rows and " << this->no_cols  << " columns " << std::endl;
    );*/
  }

public:
  
  
  virtual s_type get(const unsigned& i, const unsigned& j) const {
    return table[i * PSIZE + j];
  }
  
  virtual void add(const unsigned& i, const unsigned& j, const int& val) {
    table[i * PSIZE + j] += val;
  }

  //virtual void insert(const d_type& data, uint32_t results_m[256]) = 0;
  virtual s_type query(const d_type& data, TabularHashMerged *hashy) = 0;
  virtual double getError(std::pair<s_type, d_type>* &freq_decreasing, unsigned hitterRankThreshold, TabularHashMerged *hashy) = 0;
  virtual double printFreq(std::pair<s_type, d_type>* &freq_decreasing, unsigned hitterRankThreshold, TabularHashMerged *hashy, unsigned numt) = 0;
  virtual double printFreq2(std::pair<s_type, d_type>* &freq_decreasing, unsigned hitterRankThreshold, TabularHashMerged *hashy, unsigned numt) = 0;
  
  /*double getError(std::pair<s_type, d_type>* &freq_decreasing, unsigned hitterRankThreshold) {
    double err = 0.0;
      
    for(int i = 0; i < hitterRankThreshold; i++) {
      d_type curr = freq_decreasing[i].second;

      s_type actual = freq_decreasing[i].first;
      s_type guess = query(curr);
      // std::cout << actual << " " << guess << "\n";
      double cerr = ((double)(abs(guess - actual)) / actual);
      err += cerr * cerr;
    }
    return sqrt(err / hitterRankThreshold);
  }*/

  void print() {
    for(unsigned i = 0; i < no_rows; i++) {
      for(unsigned j = 0; j < no_cols; j++) {
	std::cout << get(i, j) << " ";
      }
      std::cout << std::endl;
    }
  }
  
  void reset() {
    memset(table, 0, sizeof(s_type) * no_rows * PSIZE);
  }

  unsigned get_no_rows() {
    return no_rows;
  }

  unsigned get_no_cols() {
    return no_cols;
  }
    
  ~CSketch_MP() {
    delete [] table;
  }
};

template <class s_type, class d_type>
class CountMinSketch_MP: public CSketch_MP<s_type, d_type> {
public:
  CountMinSketch_MP(double epsilon, double delta) : CSketch_MP<s_type, d_type>(epsilon, delta) {}
  
  /*  virtual void insert(const d_type& data, uint32_t results_m[256]) {
    (this->hash_m)->hash(data, results_m);
    //#pragma omp critical
    //{
      for(unsigned i = 0; i < this->no_rows; i++) {
	unsigned col_id = (results_m[i]) % this->no_cols;
	this->add(i, col_id, 1);
      }
      //}
      }*/

/*  virtual uint32_t hashElement(const d_type &data) {
    (this->hash_m)->hash(data, this->results_m)
    return *results_m;
  }*/

  virtual s_type query(const d_type& data, TabularHashMerged *hashy) {
    uint32_t results_m[256];
    s_type r_freq, freq = std::numeric_limits<int>::max();

    hashy->hash(data, results_m);
    
    unsigned h_col;
    for(unsigned i = 0; i < this->no_rows; i++) {
      h_col = results_m[i] % this->no_cols;
      r_freq = this->get(i, h_col);
      if(r_freq < freq) {
	freq = r_freq;
      }
    }
    return freq;
  }

  double getError(std::pair<s_type, d_type>* &freq_decreasing, unsigned hitterRankThreshold, TabularHashMerged *hashy) {
    double err = 0.0;
      
    for(int i = 0; i < hitterRankThreshold; i++) {
      d_type curr = freq_decreasing[i].second;

      s_type actual = freq_decreasing[i].first;
      s_type guess = query(curr, hashy);
      // std::cout << actual << " " << guess << "\n";
      double cerr = ((double)(abs(guess - actual)) / actual);
      err += cerr * cerr;
    }
    return sqrt(err / hitterRankThreshold);
  }
  
  double printFreq(std::pair<s_type, d_type>* &freq_decreasing, unsigned hitterRankThreshold, TabularHashMerged *hashy, unsigned numt) {
    std::cout << numt << " Threads" << std::endl;
    std::cout << "Printing " << numt << " frequencies" << std::endl;
    std::ofstream out;
    out.open("z3_nebulas_big"+std::to_string(numt) + ".txt");
    for(int i = 0; i < hitterRankThreshold; i++) {
      d_type curr = freq_decreasing[i].second;
      
      s_type actual = freq_decreasing[i].first;
      s_type guess = query(curr, hashy);
      
      out << guess<<"\n" ;
      //std::cout << guess << std::endl;
    }
    //    std::cout << "," <<std::endl;
    out.close();
  }

  double printFreq2(std::pair<s_type, d_type>* &freq_decreasing, unsigned hitterRankThreshold, TabularHashMerged *hashy, unsigned numt) {
    std::cout << numt << " Threads" << std::endl;
    std::cout << "Printing " << numt << " frequencies" << std::endl;
    std::ofstream out;
    out.open("z3_nebulabarrs_big"+std::to_string(numt) + ".txt");
    for(int i = 0; i < hitterRankThreshold; i++) {
      d_type curr = freq_decreasing[i].second;
      
      s_type actual = freq_decreasing[i].first;
      s_type guess = query(curr, hashy);
      
      out << guess<<"\n" ;
      //std::cout << guess << std::endl;
    }
    //    std::cout << "," <<std::endl;
    out.close();
  }
  
};
#endif


