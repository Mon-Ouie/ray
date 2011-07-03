#include "say.h"

static say_array *say_vertex_types = NULL;

static void say_vertex_type_delete(say_vertex_type *type) {
  say_array_free(type->elements);
}

static void say_vertex_elem_free(say_vertex_elem *elem) {
  free(elem->name);
}

static say_array *say_vertex_type_get_all() {
  if (!say_vertex_types) {
    say_vertex_types = say_array_create(sizeof(say_vertex_type),
                                        NULL,
                                        (say_destructor)say_vertex_type_delete);

    say_vertex_type type;
    type.elements = say_array_create(sizeof(say_vertex_elem),
                                     (say_destructor)say_vertex_elem_free, NULL);

    say_vertex_elem el;

    el.type = SAY_VECTOR2;
    el.name = say_strdup(SAY_POS_ATTR);
    say_vertex_type_push(&type, el);

    el.type = SAY_COLOR;
    el.name = say_strdup(SAY_COLOR_ATTR);
    say_vertex_type_push(&type, el);

    el.type = SAY_VECTOR2;
    el.name = say_strdup(SAY_TEX_COORD_ATTR);
    say_vertex_type_push(&type, el);

    say_array_push(say_vertex_types, &type);
  }

  return say_vertex_types;
}

size_t say_vertex_type_make_new() {
  say_vertex_type type;
  type.elements = say_array_create(sizeof(say_vertex_elem), NULL, NULL);

  say_array_push(say_vertex_type_get_all(), &type);
  return say_array_get_size(say_vertex_types) - 1;
}

say_vertex_type *say_get_vertex_type(size_t i) {
  return say_array_get(say_vertex_type_get_all(), i);
}

void say_vertex_type_push(say_vertex_type *type, say_vertex_elem elem) {
  say_array_push(type->elements, &elem);
}

say_vertex_elem_type say_vertex_type_get_type(say_vertex_type *type, size_t i) {
  return ((say_vertex_elem*)say_array_get(type->elements, i))->type;
}

const char *say_vertex_type_get_name(say_vertex_type *type, size_t i) {
  return ((say_vertex_elem*)say_array_get(type->elements, i))->name;
}

size_t say_vertex_type_get_elem_count(say_vertex_type *type) {
  return say_array_get_size(type->elements);
}

size_t say_vertex_type_get_size(say_vertex_type *type) {
  size_t sum = 0;
  for (say_vertex_elem *e = say_array_get(type->elements, 0);
       e;
       say_array_next(type->elements, (void**)&e)) {
    switch (e->type) {
    case SAY_FLOAT: sum += sizeof(GLfloat); continue;
    case SAY_INT:   sum += sizeof(GLint);   continue;
    case SAY_UBYTE: sum += sizeof(GLubyte); continue;
    case SAY_BOOL:  sum += sizeof(GLint);   continue;

    case SAY_COLOR:   sum += sizeof(GLubyte) * 4; continue;
    case SAY_VECTOR2: sum += sizeof(GLfloat) * 2; continue;
    case SAY_VECTOR3: sum += sizeof(GLfloat) * 3; continue;
    }
  }

  return sum;
}

size_t say_vertex_type_get_offset(say_vertex_type *type, size_t elem) {
  size_t sum = 0;
  for (size_t i = 0; i < elem; i++) {
    switch (say_vertex_type_get_type(type, i)) {
    case SAY_FLOAT: sum += sizeof(GLfloat); continue;
    case SAY_INT:   sum += sizeof(GLint);   continue;
    case SAY_UBYTE: sum += sizeof(GLubyte); continue;
    case SAY_BOOL:  sum += sizeof(GLint);   continue;

    case SAY_COLOR:   sum += sizeof(GLubyte) * 4; continue;
    case SAY_VECTOR2: sum += sizeof(GLfloat) * 2; continue;
    case SAY_VECTOR3: sum += sizeof(GLfloat) * 3; continue;
    }
  }

  return sum;
}

void say_vertex_type_clean_up() {
  if (say_vertex_types)
    say_array_free(say_vertex_types);
  say_vertex_types = NULL;
}
