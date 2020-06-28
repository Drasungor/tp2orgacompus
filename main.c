#include <string.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "constants.h"
//#define CACHE_SIZE 4096

typedef struct line {
  bool is_valid; //Esto representa el valid bit
  unsigned int tag; //nos sobran 2 bits porque el tag es de 6 bits
  char block[BLOCK_SIZE];
  unsigned int times_used;
} line_t;

typedef struct set {
  line_t lines[ASSOCIATIVITY];
} set_t;

set_t cache[NUMBER_OF_SETS];
char memory[MEMORY_SIZE];
unsigned int cache_accesses;
unsigned int number_of_misses;


bool _are_strings_equal(char* str1, char* str2) {
  if (strlen(str1) != strlen(str2)) {
    return false;
  }
  return !strcmp(str1, str2);
}


//Retorna el numero de bloque en memoria al que pertenece el address recibido
unsigned int _get_block_number(unsigned int address) {
  //Estas dos lineas consiguen el numero de bloque (separando la memoria en
    //bloques de 128 bytes) al que pertenece el address
    unsigned int address_offset = address % BLOCK_SIZE;
    return (address - address_offset) / BLOCK_SIZE;
}

unsigned int _get_tag(unsigned int address) {
  unsigned int block_number = _get_block_number(address);

  //Estas dos lineas se fijan si el numero de bloque esta alineado a 8 bloques,
  //si no lo estan entonces le resta el valor necesario para que lo sea
  unsigned int block_offset = block_number % NUMBER_OF_SETS;
  unsigned int centered_block_number = block_number - block_offset;

  //Una vez alineado a 8 bloques, se divide por 8 para saber el tag, ya que el
  //tag de un set especifico aumenta cada 8 bloques (en este cache que tiene 8 sets)
  return centered_block_number / NUMBER_OF_SETS;
}


void init() {
  memset(cache, 0, sizeof(set_t) * NUMBER_OF_SETS);
  memset(memory, 0, sizeof(char) * MEMORY_SIZE);
  cache_accesses = 0;
  number_of_misses = 0;
}

unsigned int get_offset(unsigned int address) {
  return (address % BLOCK_SIZE);
}

unsigned int find_set(unsigned int address) {
  unsigned int addressBlock = address / BLOCK_SIZE;
  return (addressBlock % NUMBER_OF_SETS);
}

unsigned int select_oldest(unsigned int setnum) {
  unsigned int oldest_index = 0;
  for (size_t i = 0; i < ASSOCIATIVITY; ++i) {
    if (cache[setnum].lines[i].times_used > cache[setnum].lines[oldest_index].times_used) {
      oldest_index = i;
    }
  }
  return oldest_index;
}

int compare_tag(unsigned int tag, unsigned int set) {
  for (int i = 0; i < ASSOCIATIVITY; ++i) {
    if (cache[set].lines[i].is_valid && cache[set].lines[i].tag == tag) return i;
  }
  return -1;
}

void read_to_cache(unsigned int blocknum, unsigned int way, unsigned int set) {
  unsigned int address = blocknum * BLOCK_SIZE;
  memcpy(cache[set].lines[way].block, memory + address, BLOCK_SIZE);
  cache[set].lines[way].is_valid = true;
  cache[set].lines[way].tag = _get_tag(address);
}



void _increase_blocks_use_count(unsigned int set) {
  for (int i = 0; i < ASSOCIATIVITY; ++i) {
    ++cache[set].lines[i].times_used;
  }
}

unsigned char read_byte(unsigned int address) {
  unsigned int set = find_set(address);
  unsigned int offset = get_offset(address);
  unsigned int tag = _get_tag(address);
  int way = compare_tag(tag, set);
  if (way == -1) {
    way = select_oldest(set);
    read_to_cache(_get_block_number(address), way, set);
    ++number_of_misses;
  }
  _increase_blocks_use_count(set);
  cache[set].lines[way].times_used = 0;
  ++cache_accesses;
  return cache[set].lines[way].block[offset];
}

void write_to_cache(unsigned int address, unsigned char value) {
  unsigned int set = find_set(address);
  unsigned int offset = get_offset(address);
  unsigned int tag = _get_tag(address);
  int way = compare_tag(tag, set);
  if (way != -1) {
    _increase_blocks_use_count(set);
    cache[set].lines[way].times_used = 0;
    cache[set].lines[way].block[offset] = value;
    ++cache_accesses;
  }
}

void write_byte(unsigned int address, unsigned char value) {
  memory[address] = value;//Escribo value en memoria
  write_to_cache(address, value);
}

float get_miss_rate() {
  printf("Cantidad de misses: %d, Cantidad de accesos: %d\n", number_of_misses, cache_accesses);
  return ((float)number_of_misses)/((float)cache_accesses);
}

//Recibe el string a comparar con lo que queda de una linea del archivo
bool _read_string(FILE* file, char* str2) {
  char c = 0;
  int i = 0;
  int string_len = strlen(str2);
  bool is_valid_command = true;
  while (c != '\n') {
    c = fgetc(file);
    if (i < string_len) {
      if (c != str2[i]) {
        is_valid_command = false;
      }
    } else if (c != '\n') {
      is_valid_command = false;
    }
    ++i;
  }
  return is_valid_command;
}

int _execute_flush(FILE* file) {
  if (!_read_string(file, REST_OF_FLUSH_COMMAND_TEXT)) {
    return ERROR_INVALID_COMMAND;
  }
  init();
  return SUCCESS;
}

int _execute_read(FILE* file) {
  int address = 0;
  int read_parameters = fscanf(file, " %d\n", &address);
  if ((read_parameters != 1) ) {
    return ERROR_INVALID_PARAMETERS_AMOUNT;
  } else if ((address < 0) || (address >= MEMORY_SIZE)) {
    return ERROR_INVALID_ADDRESS;
  }
  printf("Byte leido: %d\n", read_byte(address));
  return SUCCESS;
}

int _execute_write(FILE* file) {
  int address = 0, value = 0;
  int read_parameters = fscanf(file, " %d, %d\n", &address, &value);
  if (read_parameters !=  2){
      return ERROR_INVALID_PARAMETERS_AMOUNT;
  } else if ((address < 0) || (address > MEMORY_SIZE)) {
    return ERROR_INVALID_ADDRESS;
  } else if ((value < 0) || (value > MAX_VALUE)) {
    return ERROR_INVALID_VALUE;
  }
  write_byte(address, value);
  return SUCCESS;
}

int _execute_miss_rate(FILE* file) {
  if (!_read_string(file, REST_OF_MISS_RATE_COMMAND_TEXT)) {
    return ERROR_INVALID_COMMAND;
  }
  printf("Miss Rate: %f\n", get_miss_rate());
  return SUCCESS;
}

int _execute_command(char command_indicator, FILE* file) {
  switch (command_indicator) {
    case 'W':
      return _execute_write(file);
    case 'R':
      return _execute_read(file);
    case 'F':
      return _execute_flush(file);
    case 'M':
      return _execute_miss_rate(file);
    default:
      return ERROR;
  }
}

void _show_error(int program_status) {
  switch (program_status) {
    case ERROR_INVALID_ARGUMENTS_AMOUNT:
      fprintf(stderr, "Cantidad inválida de argumentos\n");
      break;
    case ERROR_INVALID_PARAMETERS_AMOUNT:
      fprintf(stderr, "Cantidad inválida de parametros\n");
      break;
    case ERROR_INVALID_COMMAND:
      fprintf(stderr, "Comando inválido\n");
      break;
    case ERROR_INVALID_ADDRESS:
      fprintf(stderr, "Direccion inválida\n");
      break;
    case ERROR_INVALID_VALUE:
      fprintf(stderr, "Valor inválido\n");
      break;
    default:
      fprintf(stderr, "Error desconocido\n");
  }
}

int main(int argc, char const *argv[]) {
  if (argc != ARGUMENTS_EXECUTABLE) {
    _show_error(ERROR_INVALID_ARGUMENTS_AMOUNT);
    return ERROR_INVALID_ARGUMENTS_AMOUNT;
  }
  FILE* file = fopen(argv[1], "r");

  init();

  char command_indicator = 0;
  int program_status = SUCCESS;
  command_indicator = fgetc(file);
  while ((!feof(file)) && (program_status == SUCCESS)) {
    program_status = _execute_command(command_indicator, file);
    command_indicator = fgetc(file);
  }
  if (program_status != SUCCESS) {
    _show_error(program_status);
  }

  fclose(file);

  return program_status;//Aca retornabamos siempre 0
}
