#include "mo.h"

/**
 * Dynamic array.
 */

mo_array *mo_array_create(size_t el_size) {
  mo_array *ret = malloc(sizeof(mo_array));
  mo_array_init(ret, el_size);

  return ret;
}

void mo_array_free(mo_array *ary) {
  mo_array_release(ary);
  free(ary);
}

void mo_array_init(mo_array *ary, size_t el_size) {
  ary->buffer  = malloc(el_size * 16);
  ary->size    = 0;
  ary->capa    = 16;
  ary->el_size = el_size;

  ary->init    = NULL;
  ary->release = NULL;
  ary->copy    = NULL;
}

void mo_array_release(mo_array *ary) {
  if (ary->release) {
    void *end = mo_array_end(ary);
    for (void *i = mo_array_begin(ary); i < end; mo_array_next(ary, &i))
      ary->release(i);
  }

  if (ary->buffer)
    free(ary->buffer);
}

void *mo_array_quick_at(mo_array *ary, size_t i) {
  return &((uint8_t*)ary->buffer)[i * ary->el_size];
}

void *mo_array_at(mo_array *ary, size_t i) {
  return i >= ary->size ? NULL : mo_array_quick_at(ary, i);
}

void *mo_array_begin(mo_array *ary) {
  return ary->buffer;
}

void *mo_array_end(mo_array *ary) {
  return mo_array_quick_at(ary, ary->size);
}

void mo_array_next(mo_array *ary, void **ptr) {
  *(uint8_t**)ptr += ary->el_size;
}

void mo_array_set(mo_array *ary, size_t i, void *obj) {
  assert(i < ary->size);

  void *cur = mo_array_quick_at(ary, i);
  if (ary->release) ary->release(cur);

  if (ary->copy)
    ary->copy(cur, obj);
  else
    memcpy(cur, obj, ary->el_size);
}

void mo_array_insert(mo_array *ary, size_t i, void *obj) {
  assert(i <= ary->size);

  if (++ary->size > ary->capa) {
    ary->buffer = realloc(ary->buffer, ary->capa * 2 * ary->el_size);
    ary->capa  *= 2;
  }

  void *ptr = mo_array_quick_at(ary, i);

  if (ary->size - 1 != i) {
    void *next = (void*)((uint8_t*)ptr + ary->el_size);
    memmove(next, ptr, (ary->size - i) * ary->el_size);
  }

  if (ary->copy)
    ary->copy(ptr, obj);
  else
    memcpy(ptr, obj, ary->el_size);
}

void mo_array_push(mo_array *ary, void *obj) {
  mo_array_insert(ary, ary->size, obj);
}

void mo_array_delete(mo_array *ary, size_t i) {
  assert(i < ary->size);

  void *ptr = mo_array_quick_at(ary, i);
  if (ary->release) ary->release(ptr);

  if (i != ary->size - 1) {
    void *next = (void*)((uint8_t*)ptr + ary->el_size);
    memmove(ptr, next, (ary->size - i) * ary->el_size);
  }

  ary->size--;
}

mo_array mo_array_dup(mo_array *ary) {
  mo_array ret;
  mo_array_copy(&ret, ary);

  return ret;
}

void mo_array_copy(mo_array *dst, mo_array *src) {
  dst->buffer  = malloc(src->el_size * src->size);
  dst->size    = src->size;
  dst->capa    = src->size;
  dst->el_size = src->el_size;

  dst->init    = src->init;
  dst->release = src->release;
  dst->copy    = src->copy;

  if (dst->copy) {
    void *src_end = mo_array_end(src);
    for (void *src_ptr = mo_array_begin(src), *dst_ptr = mo_array_begin(dst);
         src_ptr < src_end;
         mo_array_next(src, &src_ptr), mo_array_next(dst, &dst_ptr)) {
      dst->copy(dst_ptr, src_ptr);
    }
  }
  else
    memcpy(dst->buffer, src->buffer, dst->size * dst->el_size);
}

mo_array *mo_array_qsort(mo_array *ary, mo_cmp cmp) {
  qsort(ary->buffer, ary->size, ary->el_size, cmp);
  return ary;
}

void mo_array_resize(mo_array *ary, size_t size) {
  mo_array_reserve(ary, size);

  if (size > ary->size && ary->init) {
    void *end = mo_array_quick_at(ary, size);
    for (void *i = mo_array_quick_at(ary, ary->size); i < end;
         mo_array_next(ary, &i)) {
      ary->init(i);
    }
  }
  else if (size < ary->size && ary->release) {
    void *end = mo_array_quick_at(ary, ary->size);
    for (void *i = mo_array_quick_at(ary, size); i < end;
         mo_array_next(ary, &i)) {
      ary->release(i);
    }
  }

  ary->size = size;
}

void mo_array_reserve(mo_array *ary, size_t size) {
  if (ary->capa > size)
    return;

  ary->buffer = realloc(ary->buffer, size * ary->capa);
  ary->capa   = size;
}

void mo_array_shrink(mo_array *ary) {
  ary->buffer = realloc(ary->buffer, ary->size * ary->capa);
  ary->capa   = ary->size;
}

/**
 * String.
 */

void mo_string_init(mo_string *str) {
  mo_array_init(str, sizeof(char));
  mo_array_resize(str, 1);
  mo_string_at(str, 0) = '\0';
}

void mo_string_init_from_cstr(mo_string *str, const char *cstr) {
  mo_string_init(str);
  mo_string_replace(str, cstr);
}

void mo_string_init_from_buf(mo_string *str, const char *cstr, size_t size) {
  mo_string_init(str);
  mo_array_resize(str, size + 1);
  memcpy(mo_string_cstr(str), cstr, size);
  mo_string_at(str, size) = '\0';
}

void mo_string_replace(mo_string *str, const char *cstr) {
  mo_array_resize(str, strlen(cstr) + 1);
  strcpy(mo_string_cstr(str), cstr);
}

size_t mo_string_len(mo_string *str) {
  return str->size - 1;
}

void mo_string_append(mo_string *str, const char *cstr) {
  size_t old_size = str->size;

  mo_array_resize(str, old_size + strlen(cstr));
  strcpy(mo_array_get_ptr(str, old_size - 1, char), cstr);
}

char *mo_string_cstr(mo_string *str) {
  return (char*)str->buffer;
}

int mo_string_cmp(mo_string *a, mo_string *b) {
  return strcmp(mo_string_cstr(a), mo_string_cstr(b));
}

/**
 * Doubly linked list.
 */

void mo_list_init(mo_list *list, size_t el_size) {
  list->head = list->last = NULL;

  list->release = NULL;
  list->copy    = NULL;

  list->el_size = el_size;
}

void mo_list_release(mo_list *list) {
  mo_list_it *it = list->head;

  while (it) {
    mo_list_it *next = it->next;

    if (list->release)
      list->release(it->data);
    free(it);

    it = next;
  }
}

mo_list *mo_list_create(size_t el_size) {
  mo_list *list = malloc(sizeof(mo_list));
  mo_list_init(list, el_size);
  return list;
}

void mo_list_free(mo_list *list) {
  mo_list_release(list);
  free(list);
}

void mo_list_prepend(mo_list *list, mo_list_it *it, void *data) {
  mo_list_it *new_it = malloc(offsetof(mo_list_it, data) + list->el_size);

  new_it->next = it;

  if (it && it->prev) {
    new_it->prev   = it->prev;
    it->prev->next = new_it;
  }
  else
    new_it->prev = NULL;

  if (it)
    it->prev = new_it;

  if (!list->last)
    list->last = new_it;

  if (it == list->head)
    list->head = new_it;

  if (list->copy)
    list->copy(new_it->data, data);
  else
    memcpy(new_it->data, data, list->el_size);
}

void mo_list_insert(mo_list *list, mo_list_it *it, void *data) {
  mo_list_it *new_it = malloc(offsetof(mo_list_it, data) + list->el_size);

  new_it->prev = it;

  if (it && it->next) {
    new_it->next = it->next;
    it->next->prev = new_it;
  }
  else
    new_it->next = NULL;

  if (it)
    it->next = new_it;

  if (!list->head)
    list->head = new_it;

  if (it == list->last)
    list->last = new_it;

  if (list->copy)
    list->copy(new_it->data, data);
  else
    memcpy(new_it->data, data, list->el_size);
}

void mo_list_delete(mo_list *list, mo_list_it *it) {
  if (it->prev)
    it->prev->next = it->next;
  else /* is head */
    list->head = it->next;

  if (it->next)
    it->next->prev = it->prev;
  else /* is tail */
    list->last = it->prev;

  if (list->release)
    list->release(it->data);
  free(it);
}

void mo_list_set(mo_list *list, mo_list_it *it, void *data) {
  if (list->copy)
    list->copy(it->data, data);
  else
    memcpy(it->data, data, list->el_size);
}

/**
 * Hash table.
 */

static
void mo_hash_zero_ptr(void *ptr) {
  *(void**)ptr = NULL;
}

void mo_hash_init(mo_hash *hash, size_t key_size, size_t el_size) {
  mo_array_init(&hash->buffer, sizeof(mo_hash_list*));

  hash->buffer.init = mo_hash_zero_ptr;
  mo_array_resize(&hash->buffer, 16);

  hash->release = NULL;
  hash->copy    = NULL;

  hash->key_release = NULL;
  hash->key_copy    = NULL;
  hash->key_cmp     = NULL;

  hash->size = 0;

  hash->el_size  = el_size;
  hash->key_size = key_size;

  hash->hash_of = NULL;
}

void mo_hash_release(mo_hash *hash) {
  for (size_t i = 0; i < hash->buffer.size; i++) {
    mo_hash_list *it = mo_array_get_as(&hash->buffer, i, mo_hash_list*);

    while (it) {
      mo_hash_list *next = it->next;

      if (hash->key_release)
        hash->key_release(it->data);
      if (hash->release)
        hash->release(it->data + hash->key_size);
      free(it);

      it = next;
    }
  }

  mo_array_release(&hash->buffer);
}

mo_hash *mo_hash_create(size_t key_size, size_t el_size) {
  mo_hash *ret = malloc(sizeof(mo_hash));
  mo_hash_init(ret, key_size, el_size);
  return ret;
}

void mo_hash_free(mo_hash *hash) {
  mo_hash_release(hash);
  free(hash);
}

bool mo_hash_has_key(mo_hash *hash, void *key) {
  int id = hash->hash_of(key) % hash->buffer.size;
  mo_hash_list *it = mo_array_get_as(&hash->buffer, id, mo_hash_list*);

  for (; it; it = it->next) {
    if (hash->key_cmp(it->data, key) == 0)
      return true;
  }

  return false;
}

void *mo_hash_get(mo_hash *hash, void *key) {
  int id = hash->hash_of(key) % hash->buffer.size;
  mo_hash_list *it = mo_array_get_as(&hash->buffer, id, mo_hash_list*);

  for (; it; it = it->next) {
    if (hash->key_cmp(it->data, key) == 0)
      return it->data + hash->key_size;
  }

  return NULL;
}

static
void mo_hash_grow(mo_hash *hash) {
  /*
   * Create a bigger hash, and push the current values there.
   */
  mo_hash copy;
  mo_hash_init(&copy, hash->key_size, hash->el_size);
  mo_array_resize(&copy.buffer, hash->buffer.size * 2);

  copy.release = hash->release;
  copy.copy    = hash->copy;

  copy.key_release = hash->key_release;
  copy.key_copy    = hash->key_copy;
  copy.key_cmp     = hash->key_cmp;

  copy.hash_of = hash->hash_of;

  mo_hash_it it = mo_hash_begin(hash);
  for (; !mo_hash_it_is_end(&it); mo_hash_it_next(&it)) {
    void *key = mo_hash_it_key(&it);
    void *val = mo_hash_it_val(&it);

    mo_hash_set(&copy, key, val);
  }

  /*
   * Release the current hash, and use the copy instead.
   */
  mo_array_release(&hash->buffer);

  *hash = copy;
}

static
void mo_hash_fill_bucket(mo_hash *hash, void *store, void *key, void *data) {
  if (hash->key_copy)
    hash->key_copy((uint8_t*)store, key);
  else
    memcpy((uint8_t*)store, key, hash->key_size);

  if (hash->copy)
    hash->copy((uint8_t*)store + hash->key_size, data);
  else
    memcpy((uint8_t*)store + hash->key_size, data,
           hash->el_size);
}

void mo_hash_set(mo_hash *hash, void *key, void *data) {
  if (hash->size + 1 > hash->buffer.size)
    mo_hash_grow(hash);

  int id = hash->hash_of(key) % hash->buffer.size;
  mo_hash_list *bucket = mo_array_get_as(&hash->buffer, id, mo_hash_list*);

  hash->size += 1;

  if (!bucket) {
    bucket = malloc(offsetof(mo_hash_list, data) +
                    hash->key_size + hash->el_size);

    bucket->next = NULL;
    mo_hash_fill_bucket(hash, bucket->data, key, data);

    mo_array_get_as(&hash->buffer, id, mo_hash_list*) = bucket;
  }
  else {
    mo_hash_list *it = bucket, *last = bucket;

    while (it) {
      if (hash->key_cmp(it->data, key) == 0) {
        hash->size -= 1;

        if (hash->copy) {
          hash->copy(it->data + hash->key_size, data);
        }
        else {
          memcpy(it->data + hash->key_size, data, hash->el_size);
        }

        return;
      }

      last = it;
      it   = it->next;
    }

    last = malloc(offsetof(mo_hash_list, data) +
                  hash->key_size + hash->el_size);
    last->next = NULL;
    mo_hash_fill_bucket(hash, last->data, key, data);

    bucket->next = last;
  }
}

void mo_hash_del(mo_hash *hash, void *key) {
  int id = hash->hash_of(key) % hash->buffer.size;
  mo_hash_list *bucket = mo_array_get_as(&hash->buffer, id, mo_hash_list*);

  mo_hash_list *it = bucket, *next = bucket->next, *prev = NULL;
  while (it) {
    if (hash->key_cmp(it->data, key) == 0) {
      hash->size -= 1;

      if (prev)
        prev->next = it->next;
      else /* is head */
        mo_array_get_as(&hash->buffer, id, mo_hash_list*) = next;

      if (hash->key_release)
        hash->key_release(it->data);
      if (hash->release)
        hash->release(it->data + hash->key_size);
      free(it);

      break;
    }

    prev = it;
    it   = it->next;
  }
}

mo_hash_it mo_hash_begin(mo_hash *hash) {
  mo_hash_it ret = {
    hash,
    NULL,
    0
  };

  for (ret.id = 0; ret.id < hash->buffer.size; ret.id++) {
    mo_hash_list *bucket = mo_array_get_as(&hash->buffer, ret.id,
                                           mo_hash_list*);

    if (bucket) {
      ret.list = bucket;
      break;
    }
  }

  return ret;
}

bool mo_hash_it_is_end(mo_hash_it *it) {
  return it->list == NULL;
}

void *mo_hash_it_key(mo_hash_it *it) {
  return it->list->data;
}

void *mo_hash_it_val(mo_hash_it *it) {
  return it->list->data + it->hash->key_size;
}

void mo_hash_it_next(mo_hash_it *it) {
  if (it->list->next)
    it->list = it->list->next;
  else {
    it->list = NULL;

    for (it->id++; it->id < it->hash->buffer.size; it->id++) {
      mo_hash_list *bucket = mo_array_get_as(&it->hash->buffer, it->id,
                                             mo_hash_list*);
      if (bucket) {
        it->list = bucket;
        return;
      }
    }
  }
}

/*
 * Used in magical computations.
 */
#define MAGIC_NUMBER 2654435761

int mo_hash_of_pointer(void *ptr) {
  return (long)(*(void**)ptr) * MAGIC_NUMBER;
}

int mo_hash_pointer_cmp(const void *a, const void *b) {
  void *first = *(void**)a, *sec = *(void**)b;

  if (first > sec)      return +1;
  else if (sec > first) return -1;
  else                  return +0;
}

int mo_hash_of_u32(void *ptr) {
  return *(uint32_t*)ptr * MAGIC_NUMBER;
}

int mo_hash_u32_cmp(const void *a, const void *b) {
  uint32_t first = *(uint32_t*)a, sec = *(uint32_t*)b;

  if (first > sec)      return +1;
  else if (sec > first) return -1;
  else                  return +0;
}

int mo_hash_of_size(void *ptr) {
  return *(size_t*)ptr * MAGIC_NUMBER;
}

int mo_hash_size_cmp(const void *a, const void *b) {
  size_t first = *(size_t*)a, sec = *(size_t*)b;

  if (first > sec)      return +1;
  else if (sec > first) return -1;
  else                  return +0;
}
