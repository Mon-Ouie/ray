#include "say.h"

static void say_table_resize(say_table *table) {
  size_t current_size = table->size;
  size_t new_size = current_size * 2;

  table->pairs = realloc(table->pairs, sizeof(say_table_pair) * new_size);

  say_table_pair default_pair = {0, NULL};
  for (size_t i = current_size; i < new_size; i++)
    table->pairs[i] = default_pair;

  table->size = new_size;
}

say_table *say_table_create(say_destructor destructor) {
  say_table *table = malloc(sizeof(say_table));

  table->destructor = destructor;

  table->size = 8;

  table->pairs = malloc(sizeof(say_table_pair) * table->size);

  say_table_pair default_pair = {0, NULL};
  for (size_t i = 0; i < table->size; i++)
    table->pairs[i] = default_pair;

  return table;
}

void say_table_free(say_table *table) {
  if (table->destructor) {
    for (size_t i = 0; i < table->size; i++) {
      if (table->pairs[i].value != NULL)
        table->destructor(table->pairs[i].value);
    }
  }

  free(table->pairs);
  free(table);
}

void *say_table_get(say_table *table, uint32_t id) {
  for (size_t i = 0; i < table->size; i++) {
    if (table->pairs[i].key == id)
      return table->pairs[i].value;
  }

  return NULL;
}

void say_table_set(say_table *table, uint32_t id, void *value) {
  for (size_t i = 0; i < table->size; i++) {
    if (table->pairs[i].key == id) {
      table->pairs[i].value = value;
      return;
    }
  }

  for (size_t i = 0; i < table->size; i++) {
    if (table->pairs[i].value == NULL) {
      table->pairs[i].key   = id;
      table->pairs[i].value = value;
      return;
    }
  }

  size_t old_size = table->size;
  say_table_resize(table);

  table->pairs[old_size].key   = id;
  table->pairs[old_size].value = NULL;
}

void say_table_del(say_table *table, uint32_t id) {
  for (size_t i = 0; i < table->size; i++) {
    if (table->pairs[i].key == id) {
      if (table->pairs[i].value && table->destructor)
        table->destructor(table->pairs[i].value);

      table->pairs[i].value = NULL;
      return;
    }
  }
}
