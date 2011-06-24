#include "say.h"

say_array *say_array_create(size_t size_el, say_destructor dtor,
                            say_creator ctor) {
  say_array *ary = malloc(sizeof(say_array));

  ary->elem_size = size_el;

  ary->create  = ctor;
  ary->destroy = dtor;

  ary->buf  = malloc(ary->elem_size * 16);
  ary->capa = 16;
  ary->size = 0;

  return ary;
}

void say_array_free(say_array *ary) {
  if (ary->destroy) {
    for (void *i = say_array_get(ary, 0); i; say_array_next(ary, &i)) {
      ary->destroy(i);
    }
  }

  free(ary->buf);
  free(ary);
}

static void *say_array_get_quick(say_array *ary, size_t i) {
  return ((uint8_t*)ary->buf + (ary->elem_size * i));
}

void *say_array_get(say_array *ary, size_t i) {
  if (i >= ary->size)
    return NULL;
  else
    return say_array_get_quick(ary, i);
}

void say_array_next(say_array *ary, void **i) {
  uint8_t *val = *i;
  val += ary->elem_size;

  if (val == say_array_get_end(ary))
    *i = NULL;
  else
    *i = val;
}

void *say_array_get_end(say_array *ary) {
  return ((uint8_t*)ary->buf + (ary->elem_size * ary->size));
}

size_t say_array_get_size(say_array *ary) {
  return ary->size;
}

size_t say_array_get_elem_size(say_array *ary) {
  return ary->elem_size;
}

void say_array_resize(say_array *ary, size_t size) {
  if (ary->size == size)
    return;

  size_t old_size = ary->size;

  if (ary->capa < size) {
    ary->buf  = realloc(ary->buf, ary->elem_size * size);
    ary->capa = size;
  }

  if (ary->create) {
    for (void *i = say_array_get(ary, old_size); i; say_array_next(ary, &i)) {
      ary->create(i);
    }
  }

  ary->size = size;
}

void say_array_insert(say_array *ary, size_t n, void *elem) {
  if (ary->size + 1 > ary->capa) {
    ary->capa *= 2;
    ary->buf  = realloc(ary->buf, ary->elem_size * ary->capa);
  }

  for (size_t i = ary->size; i > n; i--) {
    memcpy(say_array_get_quick(ary, i), say_array_get_quick(ary, i - 1),
           ary->elem_size);
  }

  memcpy(say_array_get_quick(ary, n), elem, ary->elem_size);
  ary->size++;
}

void say_array_push(say_array *ary, void *elem) {
  say_array_insert(ary, ary->size, elem);
}

void say_array_delete(say_array *ary, size_t n) {
  if (ary->destroy) {
    ary->destroy(say_array_get(ary, n));
  }

  for (size_t i = n; i < ary->size - 1; i++) {
    memcpy(say_array_get_quick(ary, i), say_array_get_quick(ary, i + 1),
           ary->elem_size);
  }

  ary->size--;
}
