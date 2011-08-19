#ifndef SAY_VERTEX_TYPE_H_
#define SAY_VERTEX_TYPE_H_

#include "say_basic_type.h"

typedef enum {
  SAY_FLOAT,
  SAY_INT,
  SAY_UBYTE,
  SAY_BOOL,

  SAY_COLOR,
  SAY_VECTOR2,
  SAY_VECTOR3
} say_vertex_elem_type;

typedef struct {
  say_vertex_elem_type  type;
  char                 *name;
  bool                  per_instance;
} say_vertex_elem;

typedef struct {
  mo_array elements;
} say_vertex_type;

size_t say_vertex_type_make_new();

say_vertex_type *say_get_vertex_type(size_t i);

void say_vertex_type_push(say_vertex_type *type, say_vertex_elem elem);

say_vertex_elem_type say_vertex_type_get_type(say_vertex_type *type, size_t i);
const char *say_vertex_type_get_name(say_vertex_type *type, size_t i);
bool say_vertex_type_is_per_instance(say_vertex_type *type, size_t i);
size_t say_vertex_type_get_offset(say_vertex_type *type, size_t elem);
size_t say_vertex_type_get_elem_count(say_vertex_type *type);
size_t say_vertex_type_get_size(say_vertex_type *type);
size_t say_vertex_type_get_instance_size(say_vertex_type *type);
bool say_vertex_type_has_instance_data(say_vertex_type *type);

void say_vertex_type_clean_up();

#endif
