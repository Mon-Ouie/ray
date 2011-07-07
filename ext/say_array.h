#ifndef SAY_ARRAY_H_
#define SAY_ARRAY_H_

#include "say_basic_type.h"

typedef struct say_array {
  say_destructor destroy;
  say_destructor create;

  void   *buf;
  size_t  size;
  size_t  capa;

  size_t elem_size;
} say_array;

say_array *say_array_create(size_t size_el, say_destructor dtor,
                            say_creator ctor);
void say_array_free(say_array *ary);

void   *say_array_get(say_array *ary, size_t i);
void    say_array_next(say_array *ary, void **i);
void   *say_array_get_end(say_array *ary);
size_t  say_array_get_size(say_array *ary);
size_t  say_array_get_elem_size(say_array *ary);

void say_array_resize(say_array *ary, size_t size);
void say_array_insert(say_array *ary, size_t i, void *elem);
void say_array_push(say_array *ary, void *elem);
void say_array_delete(say_array *ary, size_t i);

#endif
