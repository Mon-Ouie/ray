#ifndef SAY_BUFFER_RENDERER_H_
#define SAY_BUFFER_RENDERER_H_

#include "say_drawable.h"
#include "say_buffer.h"

typedef struct {
  say_buffer *buffer;
  say_drawable **drawables;

  size_t size;

  size_t current_vertex;
  size_t current_drawable;
} say_buffer_renderer;

say_buffer_renderer *say_buffer_renderer_create(GLenum type,
                                                size_t vertex_count,
                                                size_t size);
void say_buffer_renderer_free(say_buffer_renderer *renderer);

void say_buffer_renderer_clear(say_buffer_renderer *renderer);
void say_buffer_renderer_push(say_buffer_renderer *renderer,
                              say_drawable *drawable);
void say_buffer_renderer_update(say_buffer_renderer *renderer);

void say_buffer_renderer_render(say_buffer_renderer *renderer,
                                say_shader *shader);

#endif
