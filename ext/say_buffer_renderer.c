#include "say.h"

static void say_buffer_renderer_resize(say_buffer_renderer *renderer,
                                       size_t size) {
  renderer->drawables = realloc(renderer->drawables,
                                sizeof(say_drawable*) * size);
  renderer->size = size;
}

static void say_buffer_renderer_resize_buffer(say_buffer_renderer *renderer,
                                              size_t size) {
  say_buffer_resize(renderer->buffer, size);
}

say_buffer_renderer *say_buffer_renderer_create(GLenum type,
                                                size_t vertex_count,
                                                size_t size) {
  say_buffer_renderer *renderer = malloc(sizeof(say_buffer_renderer));

  renderer->buffer = say_buffer_create(type, vertex_count);

  renderer->drawables = malloc(sizeof(say_drawable*) * size);
  renderer->size = size;

  renderer->current_vertex   = 0;
  renderer->current_drawable = 0;

  return renderer;
}

void say_buffer_renderer_free(say_buffer_renderer *renderer) {
  say_buffer_free(renderer->buffer);
  free(renderer->drawables);
  free(renderer);
}

void say_buffer_renderer_clear(say_buffer_renderer *renderer) {
  renderer->current_vertex   = 0;
  renderer->current_drawable = 0;
}

void say_buffer_renderer_push(say_buffer_renderer *renderer,
                              say_drawable *drawable) {
  if (renderer->size < renderer->current_drawable + 1) {
    say_buffer_renderer_resize(renderer, renderer->size * 2);
  }

  size_t new_size = renderer->current_vertex +
    say_drawable_get_vertex_count(drawable);
  size_t current_size = say_buffer_get_size(renderer->buffer);

  if (current_size * 2 < new_size)
    say_buffer_renderer_resize_buffer(renderer, new_size);
  else if (current_size < new_size)
    say_buffer_renderer_resize_buffer(renderer, current_size * 2);

  renderer->drawables[renderer->current_drawable++] = drawable;

  say_drawable_fill_buffer(drawable,
                           say_buffer_get_vertex(renderer->buffer,
                                                 renderer->current_vertex));
  renderer->current_vertex = new_size;
}

void say_buffer_renderer_update(say_buffer_renderer *renderer) {
  say_buffer_update(renderer->buffer);
}

void say_buffer_renderer_render(say_buffer_renderer *renderer,
                                say_shader *shader) {
  say_buffer_bind(renderer->buffer);

  int using_texture = 0;
  say_shader_set_int_id(shader, SAY_TEXTURE_ENABLED_LOC_ID, 0);

  size_t current_vertex = 0;
  for (size_t i = 0; i < renderer->current_drawable; i++) {
    say_drawable *drawable = renderer->drawables[i];

    if (using_texture != say_drawable_is_textured(drawable)) {
      using_texture = !using_texture;
      say_shader_set_int(shader, SAY_TEXTURE_ENABLED_ATTR, using_texture);
    }

    size_t next_vertex = say_drawable_get_vertex_count(drawable);
    if (next_vertex > say_buffer_get_size(renderer->buffer))
      return;

    say_drawable_draw_at(drawable, current_vertex, shader);

    current_vertex = next_vertex;
    if (current_vertex >= say_buffer_get_size(renderer->buffer))
      break;
  }
}
