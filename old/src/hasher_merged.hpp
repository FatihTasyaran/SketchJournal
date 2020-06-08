#ifndef H_HASHM
#define H_HASHM

#include <stdint.h>
#include <random>
#include <iostream>
#include <string.h>
#include "hasher.hpp"

class TabularHashMerged {
private:
  uint32_t table[4][2048];
  static const uint32_t mask = 0x000000ff;
  
public:
  TabularHashMerged() { 
    std::random_device r;
    std::default_random_engine eng(r());
    std::uniform_int_distribution<uint32_t> uint_dist;
    
    for(unsigned i = 0; i < 4; i++) {
      for(unsigned j = 0; j < 2048; j++) {
	table[i][j] = uint_dist(eng);
      }
    }
  }
  /*  
  void hash(const uint32_t data, uint32_t hashes[8], uint32_t modul) const {

    uint32_t i, j, c, x = data;
    uint32_t h = 0, temp;

    c = 8 * (x & mask);
    hashes[0] = table[0][c];
    hashes[1] = table[0][c + 1];
    hashes[2] = table[0][c + 2];
    hashes[3] = table[0][c + 3];
    hashes[4] = table[0][c + 4];
    hashes[5] = table[0][c + 5];
    hashes[6] = table[0][c + 6];
    results[7] = table[0][c + 7];
    x = x >> 8;

    for (i = 1; i < 4; i++) {
      c = 8 * (x & mask);

      results[0] ^= table[i][c];
      results[1] ^= table[i][c + 1];
      results[2] ^= table[i][c + 2];
      results[3] ^= table[i][c + 3];
      results[4] ^= table[i][c + 4];
      results[5] ^= table[i][c + 5];
      results[6] ^= table[i][c + 6];
      results[7] ^= table[i][c + 7];

      x = x >> 8;
    }

    results[0] = results[0] % w; 
    results[1] = results[1] % w;
    results[2] = results[2] % w;
    results[3] = results[3] % w;
    results[4] = results[4] % w;
    results[5] = results[5] % w;
    results[6] = results[6] % w;
    results[7] = results[7] % w;
  }
  
  */

  void hash(uint32_t x, uint32_t hashes[8], 
	    uint32_t table[4][2048]) {

    uint32_t c = 8 * (x & 0x000000ff);
    x = x >> 8;

    hashes[0] = table[0][c];
    hashes[1] = table[0][c + 1];
    hashes[2] = table[0][c + 2];
    hashes[3] = table[0][c + 3];
    hashes[4] = table[0][c + 4];
    hashes[5] = table[0][c + 5];
    hashes[6] = table[0][c + 6];
    hashes[7] = table[0][c + 7];

    for (uint32_t i = 1; i < 4; i++) {
      c = 8 * (x & 0x000000ff);
      x = x >> 8;

      hashes[0] ^= table[i][c];
      hashes[1] ^= table[i][c + 1];
      hashes[2] ^= table[i][c + 2];
      hashes[3] ^= table[i][c + 3];
      hashes[4] ^= table[i][c + 4];
      hashes[5] ^= table[i][c + 5];
      hashes[6] ^= table[i][c + 6];
      hashes[7] ^= table[i][c + 7];
    }
  }

  /*

  void hash(const uint32_t data, uint32_t results[8]) const {

    uint32_t i, j, c, x = data;
    uint32_t h = 0, temp;

    c = 8 * (x & mask);
    results[0] = table[0][c];
    results[1] = table[0][c + 1];
    results[2] = table[0][c + 2];
    results[3] = table[0][c + 3];
    results[4] = table[0][c + 4];
    results[5] = table[0][c + 5];
    results[6] = table[0][c + 6];
    results[7] = table[0][c + 7];
    x = x >> 8;

    for (i = 1; i < 4; i++) {
      c = 8 * (x & mask);

      results[0] ^= table[i][c];
      results[1] ^= table[i][c + 1];
      results[2] ^= table[i][c + 2];
      results[3] ^= table[i][c + 3];
      results[4] ^= table[i][c + 4];
      results[5] ^= table[i][c + 5];
      results[6] ^= table[i][c + 6];
      results[7] ^= table[i][c + 7];

      x = x >> 8;
    }
  }*?
  

  void setHashTables(TabularHash* hashes) {
    uint32_t i, j, k, *temp;
    for (i = 0; i < 8; i++) {
      for (j = 0; j < 4; j++) {
	for (k = 0; k < 256; k++) {
	  table[j][(k * 8) + i] = hashes[i].get(j,k);
	}
      }
    }
  }
  
  ~TabularHashMerged() {}
};
#endif
