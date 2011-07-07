#ifndef SAY_BUFFER_SLICE_H_
#define SAY_BUFFER_SLICE_H_

#include "say_basic_type.h"

typedef struct {
  size_t buf_id;
  size_t loc;

  size_t vtype;
  size_t size;
} say_buffer_slice;

say_buffer_slice *say_buffer_slice_create(size_t vtype, size_t size);
void say_buffer_slice_free(say_buffer_slice *slice);

void say_buffer_slice_recreate(say_buffer_slice *slice, size_t size);

size_t say_buffer_slice_get_loc(say_buffer_slice *slice);
size_t say_buffer_slice_get_size(say_buffer_slice *slice);

void *say_buffer_slice_get_vertex(say_buffer_slice *slice, size_t id);

void say_buffer_slice_update(say_buffer_slice *slice);
void say_buffer_slice_bind(say_buffer_slice *slice);

void say_buffer_slice_clean_up();

#endif
