#ifndef SAY_TABLE_H_
#define SAY_TABLE_H_

#include "say_basic_type.h"

typedef struct {
  uint32_t key;
  void *value;
} say_table_pair;

typedef struct {
  say_table_pair *pairs;
  say_destructor destructor;
  size_t size;
} say_table;

say_table *say_table_create(say_destructor destructor);
void say_table_free(say_table *table);

void *say_table_get(say_table *table, uint32_t id);
void  say_table_set(say_table *table, uint32_t id, void *value);
void  say_table_del(say_table *table, uint32_t id);

#endif
