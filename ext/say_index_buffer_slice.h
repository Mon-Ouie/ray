#ifndef SAY_INDEX_BUFFER_SLICE_H_
#define SAY_INDEX_BUFFER_SLICE_H_

#include "say_basic_type.h"

typedef struct {
  size_t buf_id;
  size_t loc;
  size_t size;
} say_index_buffer_slice;

say_index_buffer_slice *say_index_buffer_slice_create(size_t size);
void say_index_buffer_slice_free(say_index_buffer_slice *slice);

void say_index_buffer_slice_recreate(say_index_buffer_slice *slice,
                                     size_t size);

size_t say_index_buffer_slice_get_loc(say_index_buffer_slice *slice);
size_t say_index_buffer_slice_get_size(say_index_buffer_slice *slice);

GLuint *say_index_buffer_slice_get(say_index_buffer_slice *slice, size_t id);

void say_index_buffer_slice_update(say_index_buffer_slice *slice);
void say_index_buffer_slice_bind(say_index_buffer_slice *slice);

void say_index_buffer_slice_clean_up();

#endif
