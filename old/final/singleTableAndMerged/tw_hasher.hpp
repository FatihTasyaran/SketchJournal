#ifndef TWH_HASH
#define TWH_HASH

#include <stdint.h>
#include <random>
#include <iostream>

class TwistedTabularHash {
private:
  uint64_t table[4][256];
  static const uint32_t mask = 0x000000ff;
  
public:
 TwistedTabularHash() {
    std::random_device r;
    std::default_random_engine eng(r());
    std::uniform_int_distribution<uint32_t> uint_dist;
    for(unsigned i = 0; i < 4; i++) {
      for(unsigned j = 0; j < 256; j++) {
	table[i][j] = uint_dist(eng);
      }
    }
  }

  uint32_t get(uint32_t r, uint32_t c) {
    return table[r][c];
  }
  
  uint32_t hash(const uint32_t data) const {
    uint32_t i, x = data;
    uint8_t c;
    uint64_t h = 0;
    for (i = 0; i < 3; i++) {
      c = x;
      h ^= table[i][c];
      x = x >> 8;
    }
    c=x^h;
    h^=table[i][c];
    h>>=32;
  
   
    return((uint32_t) h);
  }
};
#endif
