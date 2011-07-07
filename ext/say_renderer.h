#ifndef SAY_RENDERER_H_
#define SAY_RENDERER_H_

#include "say_shader.h"
#include "say_buffer_renderer.h"

typedef struct {
  say_shader *shader;
  uint8_t using_texture;
} say_renderer;

say_renderer *say_renderer_create();
void say_renderer_free(say_renderer *renderer);

say_shader *say_renderer_get_shader(say_renderer *renderer);

void say_renderer_reset_states(say_renderer *renderer);
void say_renderer_push(say_renderer *renderer, say_drawable *drawable);
void say_renderer_push_buffer(say_renderer *renderer,
                              say_buffer_renderer *buf);

#endif
