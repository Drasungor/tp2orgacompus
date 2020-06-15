#include <string.h>

#define CACHE_SIZE 4096
#define MEMORY_SIZE 65536


typedef struct block {
  char valid_bit;
  char tag; //nos sobran 2 bits porque el tag es de 6 bits

} block_t;

typedef struct set {
  unsigned int blocks[4]; //Block == Line
} set_t;


char cache[CACHE_SIZE];
char memory[MEMORY_SIZE];
unsigned int miss_rate;
way_t ways[4];

void init() {
  memset(cache, 0, sizeof(char) * CACHE_SIZE);
  memset(memory, 0, sizeof(char) * MEMORY_SIZE);
  miss_rate = 0;
}

unsiged int get_offset(unsigned int address)
  return offset;
}

unsigned int find_set(unsigned int address) {
  return set;
}

unsigned int select_oldest(unsigned int setnum) {
  return oldest;
}

int compare_tag (unsigned int tag, unsigned int set) {
  return 1;
}

void read_tocache(unsigned int blocknum, unsigned int way, unsigned int set) {

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
