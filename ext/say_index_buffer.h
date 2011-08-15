#ifndef SAY_INDEX_BUFFER_H_
#define SAY_INDEX_BUFFER_H_

#include "say_basic_type.h"

typedef struct {
  GLuint ibo;
  GLenum type;

  mo_array buffer;
} say_index_buffer;

say_index_buffer *say_index_buffer_create(GLenum type, size_t size);
void say_index_buffer_free(say_index_buffer *buf);

void say_index_buffer_bind(say_index_buffer *buf);
void say_index_buffer_unbind();
void say_index_buffer_rebind();

void say_index_buffer_update_part(say_index_buffer *buf, size_t index,
                                  size_t size);
void say_index_buffer_update(say_index_buffer *buf);

size_t say_index_buffer_get_size(say_index_buffer *buf);
void   say_index_buffer_resize(say_index_buffer *buf, size_t size);

GLuint *say_index_buffer_get(say_index_buffer *buf, size_t i);

#endif
