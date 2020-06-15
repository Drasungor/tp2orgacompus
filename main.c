#include <string.h>
#include "constants.h"
//#define CACHE_SIZE 4096

typedef struct line {
  bool is_valid; //Esto representa el valid bit
  unsigned int tag; //nos sobran 2 bits porque el tag es de 6 bits
  char block[BLOCK_SIZE];
} line_t;

typedef struct set {
  line_t lines[ASSOCIATIVITY];
} set_t;

set_t cache[NUMBER_OF_SETS];
char memory[MEMORY_SIZE];
unsigned int miss_rate;

void init() {
  memset(cache, 0, sizeof(set_t) * NUMBER_OF_SETS);
  memset(memory, 0, sizeof(char) * MEMORY_SIZE);
  miss_rate = 0;
}

unsiged int get_offset(unsigned int address) {

}

unsigned int find_set(unsigned int address) {

}

unsigned int select_oldest(unsigned int setnum) {
  return oldest;
}

int compare_tag(unsigned int tag, unsigned int set) {
  for (int i = 0; i < ASSOCIATIVITY; i++) {
    if (cache[set].lines[i].tag == tag) return i;
  }
  return -1;
}

void read_to_cache(unsigned int blocknum, unsigned int way, unsigned int set) {
  char* block = memory[blocknum * BLOCK_SIZE];
  memcpy(cache[set].lines[way].block, block, BLOCK_SIZE);
}

void write_to_cache(unsigned int address, unsigned char value) {

}

unsigned char read_byte(unsigned int address) {

}

void write_byte(unsigned int address, unsigned char value) {

}

float get_miss_rate() {
  return miss_rate;
}

int main(int argc, char const *argv[]) {
  /* code */
  return 0;
}
