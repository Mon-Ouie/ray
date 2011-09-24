#ifndef MY_OWN_HEADER_GUARD
#define MY_OWN_HEADER_GUARD 1

/**
 * My Own utilities.
 * (Depending on My Own mood, may mean "Mon_Ouie's utilities")
 *
 * Includes the data stuctures that I usually need when programming in C.
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#if 0
}
#endif

/**
 * Function pointer types
 */

typedef void (*mo_init)(void *dst);
typedef void (*mo_release)(void *dst);
typedef void (*mo_copy)(void *dst, void *src);
typedef int  (*mo_cmp)(const void *a, const void *b);
typedef int  (*mo_hash_func)(void *obj);

/**
 * Dynamic array.
 */

typedef struct mo_array {
  void   *buffer;
  size_t  size;
  size_t  capa;
  size_t  el_size;

  mo_init    init;
  mo_release release;
  mo_copy    copy;
} mo_array;

mo_array *mo_array_create(size_t el_size);
void mo_array_free(mo_array *ary);

void mo_array_init(mo_array *ary, size_t el_size);
void mo_array_release(mo_array *ary);

void *mo_array_quick_at(mo_array *ary, size_t i);
void *mo_array_at(mo_array *ary, size_t i);

#define mo_array_get_ptr(ary, i, type) ( (type*)mo_array_at(ary, i))
#define mo_array_get_as(ary, i, type)  (*(type*)mo_array_at(ary, i))

void *mo_array_begin(mo_array *ary);
void *mo_array_end(mo_array *ary);

void mo_array_next(mo_array *ary, void **ptr);

void mo_array_set(mo_array *ary, size_t i, void *obj);

void mo_array_insert(mo_array *ary, size_t i, void *obj);
void mo_array_push(mo_array *ary, void *obj);
void mo_array_delete(mo_array *ary, size_t i);

mo_array mo_array_dup(mo_array *ary);
void     mo_array_copy(mo_array *dst, mo_array *src);

mo_array *mo_array_qsort(mo_array *ary, mo_cmp cmp);

void mo_array_resize(mo_array *ary, size_t size);
void mo_array_reserve(mo_array *ary, size_t size);
void mo_array_shrink(mo_array *ary);

/**
 * String.
 */

typedef mo_array mo_string;

void    mo_string_init(mo_string *str);
void    mo_string_init_from_cstr(mo_string *str, const char *cstr);
void    mo_string_init_from_buf(mo_string *str, const char *cstr, size_t size);
void    mo_string_replace(mo_string *str, const char *cstr);
size_t  mo_string_len(mo_string *str);
void    mo_string_append(mo_string *str, const char *cstr);
char   *mo_string_cstr(mo_string *str);
int     mo_string_cmp(mo_string *a, mo_string *b);

#define mo_string_at(str, i) mo_array_get_as(str, i, char)

typedef struct mo_list_it {
  struct mo_list_it *prev, *next;
  uint8_t data[];
} mo_list_it;

typedef struct mo_list {
  mo_list_it *head;
  mo_list_it *last;

  mo_release release;
  mo_copy    copy;

  size_t el_size;
} mo_list;

void mo_list_init(mo_list *list, size_t el_size);
void mo_list_release(mo_list *list);

mo_list *mo_list_create(size_t el_size);
void mo_list_free(mo_list *list);

void mo_list_prepend(mo_list *list, mo_list_it *it, void *data);
void mo_list_insert(mo_list *list, mo_list_it *it, void *data);
void mo_list_delete(mo_list *list, mo_list_it *it);

void mo_list_set(mo_list *list, mo_list_it *it, void *data);

#define mo_list_it_data_ptr(it, type) ( (type*)it->data)
#define mo_list_it_data_as(it, type)  (*(type*)it->data)

/**
 * Hash table.
 */

typedef struct mo_hash_list {
  struct mo_hash_list *next;
  uint8_t data[];
} mo_hash_list;

typedef struct mo_hash {
  mo_array buffer;

  mo_release release;
  mo_copy    copy;

  mo_release key_release;
  mo_copy    key_copy;
  mo_cmp     key_cmp;

  size_t size;

  size_t el_size;
  size_t key_size;

  mo_hash_func hash_of;
} mo_hash;

typedef struct mo_hash_it {
  mo_hash      *hash;
  mo_hash_list *list;
  size_t        id;
} mo_hash_it;

int mo_hash_of_pointer(void *ptr);
int mo_hash_pointer_cmp(const void *a, const void *b);

int mo_hash_of_u32(void *ptr);
int mo_hash_u32_cmp(const void *a, const void *b);

int mo_hash_of_size(void *ptr);
int mo_hash_size_cmp(const void *a, const void *b);

void mo_hash_init(mo_hash *hash, size_t key_size, size_t el_size);
void mo_hash_release(mo_hash *hash);

mo_hash *mo_hash_create(size_t key_size, size_t el_size);
void     mo_hash_free(mo_hash *hash);

bool mo_hash_has_key(mo_hash *hash, void *key);

void *mo_hash_get(mo_hash *hash, void *key);
void  mo_hash_set(mo_hash *hash, void *key, void *data);
void  mo_hash_del(mo_hash *hash, void *key);

#define mo_hash_get_ptr(hash, key, type) ( (type*)mo_hash_get(hash, key))
#define mo_hash_get_as(hash, key, type)  (*(type*)mo_hash_get(hash, key))

mo_hash_it mo_hash_begin(mo_hash *hash);

bool mo_hash_it_is_end(mo_hash_it *it);

void *mo_hash_it_key(mo_hash_it *it);
void *mo_hash_it_val(mo_hash_it *it);

#define mo_hash_it_key_ptr(it, type) ( (type*)mo_hash_it_key(it))
#define mo_hash_it_key_as(it, type)  (*(type*)mo_hash_it_key(it))

#define mo_hash_it_val_ptr(it, type) ( (type*)mo_hash_it_val(it))
#define mo_hash_it_val_as(it, type)  (*(type*)mo_hash_it_val(it))

void mo_hash_it_next(mo_hash_it *it);

/**
 * Set (macros atop a hash table).
 */

typedef mo_hash    mo_set;
typedef mo_hash_it mo_set_it;

#define mo_set_init(set, key_size, el_size) mo_hash_init(set, key_size, el_size)
#define mo_set_release(set)                 mo_hash_init(set)

#define mo_set_create(size) mo_hash_create(size, 0)
#define mo_set_free(set)    mo_hash_delete(set)

#define mo_set_includes(set) mo_hash_has_key(set)

#define mo_set_add(set, el) mo_hash_set(set, el, NULL)
#define mo_set_del(set, el) mo_hash_del(set, el)

#define mo_set_begin(set) mo_hash_begin(set)

#define mo_set_it_is_end(it) mo_hash_it_is_end(it)
#define mo_set_it_val(it)    mo_hash_it_key(it)
#define mo_set_it_next(it)   mo_hash_it_next(it)

#define mo_set_it_val_ptr(it, type) mo_hash_it_key_ptr(it, type)
#define mo_set_it_val_as(it, type)  mo_hash_it_key_as(it, type)

#ifdef __cplusplus
}
#endif

#endif /* MY OWN HEADER'S GUARD! */
