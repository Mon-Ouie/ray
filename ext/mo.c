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
  if (ary->release && ary->size > 0) {
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

  ary->buffer = realloc(ary->buffer, size * ary->el_size);
  ary->capa   = size;
}

void mo_array_shrink(mo_array *ary) {
  if (ary->capa == ary->size) return;
  ary->buffer = realloc(ary->buffer, ary->size * ary->el_size);
  ary->capa   = ary->size;
}

/**
 * Doubly linked list.
 */

mo_list *mo_list_create(size_t el_size) {
  mo_list *list = malloc(sizeof(mo_list) + el_size);

  list->prev = list->next = NULL;

  list->release = NULL;
  list->copy    = NULL;

  list->el_size = el_size;

  return list;
}

void mo_list_free(mo_list *list) {
  do {
    mo_list *next = list->next;

    if (list->release)
      list->release(list->data);
    free(list);

    list = next;
  } while (list);
}

mo_list *mo_list_prepend(mo_list *list, void *data) {
  mo_list *prev = malloc(sizeof(mo_list) + list->el_size);

  prev->prev = list->prev;
  prev->next = list;

  if (list->prev)
    list->prev->next = list;

  list->prev = prev;

  prev->release = list->release;
  prev->copy    = list->copy;

  prev->el_size = list->el_size;

  if (prev->copy)
    prev->copy(prev->data, data);
  else if (data)
    memcpy(prev->data, data, prev->el_size);

  return prev;
}

mo_list *mo_list_insert(mo_list *list, void *data) {
  mo_list *next = malloc(sizeof(mo_list) + list->el_size);

  next->prev = list;
  next->next = list->next;

  if (list->next)
    list->next->prev = next;

  list->next = next;

  next->release = list->release;
  next->copy    = list->copy;

  next->el_size = list->el_size;

  if (next->copy)
    next->copy(next->data, data);
  else if (data)
    memcpy(next->data, data, next->el_size);

  return next;
}

mo_list *mo_list_delete(mo_list *list) {
  if (list->prev)
    list->prev->next = list->next;

  if (list->next)
    list->next->prev = list->prev;

  mo_list *next = list->next;

  if (list->release)
    list->release(list->data);
  free(list);

  return next;
}

void mo_list_set(mo_list *list, void *data) {
  if (list->copy)
    list->copy(list->data, data);
  else
    memcpy(list->data, data, list->el_size);
}

/**
 * Hash table.
 */

static
void mo_hash_zero_ptr(void *ptr) {
  *(void**)ptr = NULL;
}

static
void mo_hash_list_free(void *ptr) {
  mo_list *list = *(mo_list**)ptr;
  if (list) mo_list_free(list);
}

static
void mo_hash_bucket_free(void *ptr) {
  uint8_t *start = ptr;

  mo_hash *hash = *(mo_hash**)start;
  if (!hash)
    return;

  if (hash->key_release)
    hash->key_release(start + sizeof(mo_hash*));

  if (hash->release)
    hash->release(start + sizeof(mo_hash*) + hash->key_size);
}

static
void mo_hash_init(mo_hash *hash, size_t key_size, size_t el_size) {
  mo_array_init(&hash->buffer, sizeof(mo_list*));

  hash->buffer.init    = mo_hash_zero_ptr;
  mo_array_resize(&hash->buffer, 16);
  hash->buffer.release = mo_hash_list_free;

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

static
void mo_hash_release(mo_hash *hash) {
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
  mo_list *bucket = mo_array_get_as(&hash->buffer, id, mo_list*);

  while (bucket) {
    if (hash->key_cmp(bucket->data + sizeof(mo_hash*), key) == 0)
      return true;
    bucket = bucket->next;
  }

  return false;
}

void *mo_hash_get(mo_hash *hash, void *key) {
  int id = hash->hash_of(key) % hash->buffer.size;
  mo_list *bucket = mo_array_get_as(&hash->buffer, id, mo_list*);

  while (bucket) {
    if (hash->key_cmp(bucket->data + sizeof(mo_hash*), key) == 0)
      return bucket->data + sizeof(mo_hash*) + hash->key_size;
    bucket = bucket->next;
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
  mo_hash_release(hash);
  *hash = copy;

  /*
   * Because each element actually contains a pointer to the hash, we need to
   * update it.
   */

  it = mo_hash_begin(hash);
  for (; !mo_hash_it_is_end(&it); mo_hash_it_next(&it)) {
    mo_hash **data = (mo_hash**)(it.list->data);
    *data          = hash;
  }
}

static
void mo_hash_fill_bucket(mo_hash *hash, void *store, void *key, void *data) {
  *(mo_hash**)store = hash;

  if (hash->key_copy)
    hash->key_copy((uint8_t*)store + sizeof(mo_hash*), key);
  else
    memcpy((uint8_t*)store + sizeof(mo_hash*), key, hash->key_size);

  if (hash->copy)
    hash->copy((uint8_t*)store + sizeof(mo_hash*) + hash->key_size, data);
  else
    memcpy((uint8_t*)store + sizeof(mo_hash*) + hash->key_size, data,
           hash->el_size);
}

void mo_hash_set(mo_hash *hash, void *key, void *data) {
  if (hash->size + 1 > hash->buffer.size)
    mo_hash_grow(hash);

  int id = hash->hash_of(key) % hash->buffer.size;
  mo_list *bucket = mo_array_get_as(&hash->buffer, id, mo_list*);

  hash->size += 1;

  if (!bucket) {
    bucket = mo_list_create(sizeof(mo_hash*) + hash->key_size + hash->el_size);
    bucket->release = mo_hash_bucket_free;

    mo_hash_fill_bucket(hash, bucket->data, key, data);

    mo_array_get_as(&hash->buffer, id, mo_list*) = bucket;
  }
  else {
    mo_list *it = bucket, *last = bucket;
    while (it) {
      if (hash->key_cmp(it->data + sizeof(mo_hash*), key) == 0) {
        hash->size -= 1;

        if (hash->copy) {
          hash->copy(it->data + sizeof(mo_hash*) + hash->key_size, data);
        }
        else {
          memcpy(it->data + sizeof(mo_hash*) + hash->key_size, data,
                 hash->el_size);
        }

        return;
      }

      last = it;
      it   = it->next;
    }

    last = mo_list_insert(last, NULL);
    mo_hash_fill_bucket(hash, last->data, key, data);
  }
}

void mo_hash_del(mo_hash *hash, void *key) {
  int id = hash->hash_of(key) % hash->buffer.size;
  mo_list *bucket = mo_array_get_as(&hash->buffer, id, mo_list*);

  mo_list *it = bucket, *next = bucket->next;
  while (it) {
    if (hash->key_cmp(it->data + sizeof(mo_hash*), key) == 0) {
      hash->size -= 1;
      mo_list_delete(it);
      break;
    }

    it = it->next;
  }

  if (it == bucket) /* Head changed */
    mo_array_get_as(&hash->buffer, id, mo_list*) = next;
}

mo_hash_it mo_hash_begin(mo_hash *hash) {
  mo_hash_it ret = {
    hash,
    NULL,
    0
  };

  for (ret.id = 0; ret.id < hash->buffer.size; ret.id++) {
    mo_list *bucket = mo_array_get_as(&hash->buffer, ret.id, mo_list*);
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
  return it->list->data + sizeof(mo_hash*);
}

void *mo_hash_it_val(mo_hash_it *it) {
  return it->list->data + sizeof(mo_hash*) + it->hash->key_size;
}

void mo_hash_it_next(mo_hash_it *it) {
  if (it->list->next)
    it->list = it->list->next;
  else {
    it->list = NULL;

    for (it->id++; it->id < it->hash->buffer.size; it->id++) {
      mo_list *bucket = mo_array_get_as(&it->hash->buffer, it->id, mo_list*);
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
  return (*(uint32_t*)ptr) * MAGIC_NUMBER;
}

int mo_hash_u32_cmp(const void *a, const void *b) {
  uint32_t first = *(uint32_t*)a, sec = *(uint32_t*)b;

  if (first > sec)      return +1;
  else if (sec > first) return -1;
  else                  return +0;
}

int mo_hash_of_size(void *ptr) {
  return (*(size_t*)ptr) * MAGIC_NUMBER;
}

int mo_hash_size_cmp(const void *a, const void *b) {
  size_t first = *(size_t*)a, sec = *(size_t*)b;

  if (first > sec)      return +1;
  else if (sec > first) return -1;
  else                  return +0;
}
