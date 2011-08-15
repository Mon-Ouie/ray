#include "say.h"

static mo_array *say_vertex_types = NULL;

static void say_vertex_type_delete(say_vertex_type *type) {
  mo_array_release(&type->elements);
}

static void say_vertex_elem_free(say_vertex_elem *elem) {
  free(elem->name);
}

static mo_array *say_vertex_type_get_all() {
  if (!say_vertex_types) {
    say_vertex_types = mo_array_create(sizeof(say_vertex_type));
    say_vertex_types->release = (say_destructor)say_vertex_type_delete;

    say_vertex_type type;
    mo_array_init(&type.elements, sizeof(say_vertex_elem));
    type.elements.release = (say_destructor)say_vertex_elem_free;

    say_vertex_elem el;

    el.per_instance = false;

    el.type = SAY_VECTOR2;
    el.name = say_strdup(SAY_POS_ATTR);
    say_vertex_type_push(&type, el);

    el.type = SAY_COLOR;
    el.name = say_strdup(SAY_COLOR_ATTR);
    say_vertex_type_push(&type, el);

    el.type = SAY_VECTOR2;
    el.name = say_strdup(SAY_TEX_COORD_ATTR);
    say_vertex_type_push(&type, el);

    mo_array_push(say_vertex_types, &type);
  }

  return say_vertex_types;
}

static size_t say_vertex_type_size_of(say_vertex_elem_type type) {
  switch (type) {
  case SAY_FLOAT: return sizeof(GLfloat);
  case SAY_INT:   return sizeof(GLint);
  case SAY_UBYTE: return sizeof(GLubyte);
  case SAY_BOOL:  return sizeof(GLint);

  case SAY_COLOR:   return(sizeof(GLubyte) * 4);
  case SAY_VECTOR2: return(sizeof(GLfloat) * 2);
  case SAY_VECTOR3: return(sizeof(GLfloat) * 3);
  }

  return 0;
}

size_t say_vertex_type_make_new() {
  say_vertex_type type;
  mo_array_init(&type.elements, sizeof(say_vertex_elem));
  type.elements.release = (say_destructor)say_vertex_elem_free;

  mo_array_push(say_vertex_type_get_all(), &type);
  return say_vertex_types->size - 1;
}

say_vertex_type *say_get_vertex_type(size_t i) {
  return mo_array_at(say_vertex_type_get_all(), i);
}

void say_vertex_type_push(say_vertex_type *type, say_vertex_elem elem) {
  mo_array_push(&type->elements, &elem);
}

say_vertex_elem_type say_vertex_type_get_type(say_vertex_type *type, size_t i) {
  return mo_array_get_as(&type->elements, i, say_vertex_elem).type;
}

const char *say_vertex_type_get_name(say_vertex_type *type, size_t i) {
  return mo_array_get_as(&type->elements, i, say_vertex_elem).name;
}

bool say_vertex_type_is_per_instance(say_vertex_type *type, size_t i) {
  return mo_array_get_as(&type->elements, i, say_vertex_elem).per_instance;
}

size_t say_vertex_type_get_elem_count(say_vertex_type *type) {
  return type->elements.size;
}

size_t say_vertex_type_get_size(say_vertex_type *type) {
  size_t sum = 0;

  say_vertex_elem *end = mo_array_end(&type->elements);
  for (say_vertex_elem *e = mo_array_at(&type->elements, 0);
       e < end;
       mo_array_next(&type->elements, (void**)&e)) {
    if (e->per_instance)
      continue;
    sum += say_vertex_type_size_of(e->type);
  }

  return sum;
}

size_t say_vertex_type_get_instance_size(say_vertex_type *type) {
  size_t sum = 0;

  say_vertex_elem *end = mo_array_end(&type->elements);
  for (say_vertex_elem *e = mo_array_at(&type->elements, 0);
       e < end;
       mo_array_next(&type->elements, (void**)&e)) {
    if (!e->per_instance)
      continue;
    sum += say_vertex_type_size_of(e->type);
  }

  return sum;
}

size_t say_vertex_type_get_offset(say_vertex_type *type, size_t elem) {
  bool per_instance = say_vertex_type_is_per_instance(type, elem);

  size_t sum = 0;
  for (size_t i = 0; i < elem; i++) {
    if (per_instance != say_vertex_type_is_per_instance(type, i))
      continue;
    sum += say_vertex_type_size_of(say_vertex_type_get_type(type, i));
  }

  return sum;
}

bool say_vertex_type_has_instance_data(say_vertex_type *type) {
  say_vertex_elem *end = mo_array_end(&type->elements);
  for (say_vertex_elem *e = mo_array_at(&type->elements, 0);
       e < end;
       mo_array_next(&type->elements, (void**)&e)) {
    if (e->per_instance)
      return true;
  }

  return false;
}

void say_vertex_type_clean_up() {
  if (say_vertex_types)
    mo_array_free(say_vertex_types);
  say_vertex_types = NULL;
}
