##ifndef _SET_STACK_H_
#define  _SET_STACK_H_

typedef struct set_stack {
  char block_index[ASSOCIATIVITY];
} set_stack_t;

void set_stack_init(set_stack_t& stack);

//Retorna el indice del bloque que fue usado hace mas tiempo
char get_least_recently_used(set_stack_t& stack);

#endif
