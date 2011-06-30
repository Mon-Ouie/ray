#include "say.h"

say_renderer *say_renderer_create() {
  say_renderer *renderer = (say_renderer*)malloc(sizeof(say_renderer));

  renderer->shader = say_shader_create();
  say_renderer_reset_states(renderer);

  return renderer;
}

void say_renderer_free(say_renderer *renderer) {
  say_shader_free(renderer->shader);
  free(renderer);
}

say_shader *say_renderer_get_shader(say_renderer *renderer) {
  return renderer->shader;
}

void say_renderer_reset_states(say_renderer *renderer) {
  renderer->using_texture = 0;
  say_shader_set_int_id(renderer->shader, SAY_TEXTURE_ENABLED_LOC_ID, 0);

}
void say_renderer_push(say_renderer *renderer, say_drawable *drawable) {
  if (!drawable->shader &&
      renderer->using_texture != say_drawable_is_textured(drawable)) {
    renderer->using_texture = !(renderer->using_texture);
    say_shader_set_int_id(renderer->shader, SAY_TEXTURE_ENABLED_LOC_ID,
                          renderer->using_texture);
  }

  say_drawable_draw(drawable, renderer->shader);
}

void say_renderer_push_buffer(say_renderer *renderer,
                              say_buffer_renderer *buf) {
  say_buffer_renderer_render(buf, renderer->shader);

  renderer->using_texture = 0;
  say_shader_set_int_id(renderer->shader, SAY_TEXTURE_ENABLED_LOC_ID, 0);
}
