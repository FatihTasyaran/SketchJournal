#ifndef CSKETCH_H
#define CSKETCH_H

#include <iostream>
#include <algorithm>
#include "hasher.hpp"
#include "common.h"
#include "omp.h"

template <class s_type, class d_type>
class CSketch {

protected:
  unsigned no_rows;
  unsigned no_cols;
  TabularHash* hashes;
  s_type* table;

  CSketch(double epsilon, double delta) {
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
    
    table = new s_type[this->no_rows * this->no_cols];
    /*
    D(
    std::cout << "A sketch is created with " << this->no_rows << " rows and " << this->no_cols  << " columns " << std::endl;
    );*/
  }

public:
  virtual void set_hashes(TabularHash* hashes) {
    this->hashes = hashes;
  }
  
  virtual s_type get(const unsigned& i, const unsigned& j) const {
    return table[i * no_cols + j];
  }
  
  virtual void add(const unsigned& i, const unsigned& j, const int& val) {
    table[i * no_cols + j] += val;
  }

  virtual void insert_to_row(const d_type& data, unsigned row_id) = 0;
  virtual void insert(const d_type& data) = 0;
  virtual s_type query(const d_type& data) const = 0;

  void insert_stream(dtype* data, unsigned long long no_stream, unsigned no_threads) {
#pragma omp parallel num_threads(no_threads)
    {
#pragma omp for
      for(unsigned i = 0; i < no_rows; i++) {
	for(unsigned long long j = 0; j < no_stream; j++) {
	  insert_to_row(data[j], i);
	}
      }
    }
  }
  
  double getError(std::pair<s_type, d_type>* &freq_decreasing, unsigned hitterRankThreshold) {
    double err = 0.0;
      
    for(int i = 0; i < hitterRankThreshold; i++) {
      d_type curr = freq_decreasing[i].second;

      s_type actual = freq_decreasing[i].first;
      s_type guess = query(curr);
      
      double cerr = ((double)(abs(guess - actual)) / actual);
      //std::cout << i << "\t" << guess << " " << actual << " " << cerr << std::endl;      
      err += cerr * cerr;
    }
    return sqrt(err / hitterRankThreshold);
  }

  void print() {
    for(unsigned i = 0; i < no_rows; i++) {
      for(unsigned j = 0; j < no_cols; j++) {
	std::cout << get(i, j) << " ";
      }
      std::cout << std::endl;
    }
  }
  
  void reset() {
    memset(table, 0, sizeof(s_type) * no_rows * no_cols);
  }

  unsigned get_no_rows() {
    return no_rows;
  }

  unsigned get_no_cols() {
    return no_cols;
  }
    
  ~CSketch() {
    delete [] table;
  }
};

template <class s_type, class d_type>
class CountMinSketch : public CSketch<s_type, d_type> {
public:
  CountMinSketch(double epsilon, double delta) : CSketch<s_type, d_type>(epsilon, delta) {}

  virtual void insert_to_row(const d_type& data, unsigned row_id) {
    unsigned col_id = this->hashes[row_id].hash(data) % this->no_cols;
    this->add(row_id, col_id, 1);
  }
  
  virtual void insert(const d_type& data) {
    for(unsigned i = 0; i < this->no_rows; i++) {
      insert_to_row(data, i);
    }   
  }

  virtual s_type query(const d_type& data) const {
    s_type r_freq, freq = std::numeric_limits<int>::max();

    unsigned h_col;
    for(unsigned i = 0; i < this->no_rows; i++) {
      h_col = this->hashes[i].hash(data) % this->no_cols;
      r_freq = this->get(i, h_col);
      if(r_freq < freq) {
	freq = r_freq;
      }
    }
    return freq;
  }
};

template <class s_type, class d_type>
class CountMinSketchRestricted : public CSketch<s_type, d_type> {
private:
  unsigned* h_vals;
  
public:
  CountMinSketchRestricted(double epsilon, double delta) : CSketch<s_type, d_type>(epsilon, delta) {
    h_vals = new unsigned[this->no_rows];
  }

  virtual void insert_to_row(const d_type& data, unsigned row_id) {
    unsigned col_id = this->hashes[row_id].hash(data) % this->no_cols;
    this->add(row_id, col_id, 1);
  }
  
  virtual void insert(const d_type& data) {
    s_type r_freq, min_freq = std::numeric_limits<int>::max();
    unsigned h_col;

    for(unsigned i = 0; i < this->no_rows; i++) {
      h_vals[i] = h_col = this->hashes[i].hash(data) % this->no_cols;
      r_freq = this->get(i, h_col);
      if(r_freq < min_freq) {
	min_freq = r_freq;
      }
    }

    for(unsigned i = 0; i < this->no_rows; i++) {
      h_col = h_vals[i];
      if(this->get(i, h_col) == min_freq) {
	this->add(i, h_col, 1);
      }
    }
  }

  virtual s_type query(const d_type& data) const {
    s_type r_freq, freq = std::numeric_limits<int>::max();

    unsigned h_col;
    for(unsigned i = 0; i < this->no_rows; i++) {
      h_col = this->hashes[i].hash(data) % this->no_cols;
      r_freq = this->get(i, h_col);
      if(r_freq < freq) {
	freq = r_freq;
      }
    }
    return freq;
  }

  ~CountMinSketchRestricted() {
    delete [] h_vals;
  }
};


template <class s_type, class d_type>
class CountMinMeanSketch : public CountMinSketch<s_type, d_type> {
private:
  s_type* results;
  
public:
  CountMinMeanSketch(double epsilon, double delta) : CountMinSketch<s_type, d_type>(epsilon, delta) {
    results = new s_type[this->no_rows];
  }
  
  virtual s_type query(const d_type& data) const {
    s_type no_stream, r_freq, noise;
    
    unsigned h_col;
    memset(results, 0, this->no_rows * sizeof(s_type));
    for(unsigned i = 0; i < this->no_rows; i++) {
      h_col = this->hashes[i].hash(data) % this->no_cols;
      r_freq = this->get(i, h_col);

      no_stream = 0;
      for(unsigned j = 0; j < this->no_cols; j++) {
	no_stream += this->get(i, j);
      }
      
      noise = ((no_stream - r_freq) / (this->no_cols - 1));
      if(r_freq > noise) {
	results[i] = r_freq - noise;
      }
    }
    std::sort(results, results + this->no_rows);
    return results[this->no_rows / 2];
  }

  ~CountMinMeanSketch() {
    delete [] results;
  }
};

template <class s_type, class d_type>
class CountSketch : public CSketch<s_type, d_type> {
private:
  TabularHash* g_hashes;
  s_type* results;
  
public:
  CountSketch(double epsilon, double delta) : CSketch<s_type, d_type>(epsilon, delta) {
    results = new s_type[this->no_rows];
  }

  virtual void insert_to_row(const d_type& data, unsigned row_id) {
    unsigned col_id = this->hashes[row_id].hash(data) % this->no_cols;
    int contrib = 2 * (g_hashes[row_id].hash(data) % 2) - 1; 
    this->add(row_id, col_id, contrib); 
  }

  virtual void insert(const d_type& data) {
    for(unsigned i = 0; i < this->no_rows; i++) {
      insert_to_row(data, i);
    }   
  }

  virtual s_type query(const d_type& data) const {
    unsigned h_col, coef;
    memset(results, 0, this->no_rows * sizeof(s_type));
    for(unsigned i = 0; i < this->no_rows; i++) {
      h_col = this->hashes[i].hash(data) % this->no_cols;
      coef = 2 * (g_hashes[i].hash(data) % 2) - 1;
      results[i] = this->get(i, h_col) * coef;
      if(results[i] < 0) results[i] = 0;
    }
    std::sort(results, results + this->no_rows);
    return results[this->no_rows / 2];
  }

  void set_g_hashes(TabularHash* g_hashes) {
    this->g_hashes = g_hashes;
  }
  
  ~CountSketch() {
    delete [] results;
  }
};
#endif
