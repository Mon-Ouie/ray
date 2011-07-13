#ifndef SAY_TARGET_H_
#define SAY_TARGET_H_

#include "say_context.h"
#include "say_renderer.h"
#include "say_view.h"
#include "say_thread.h"

typedef say_context *(*say_context_proc)(void *data);
typedef void (*say_bind_hook)(void *data);

typedef struct {
  say_thread_variable *context;
  say_context_proc context_proc;
  say_bind_hook bind_hook;

  uint8_t own_context_needed;

  void *data;

  say_renderer *renderer;

  say_view *view;
  say_vector2 size;

  uint8_t up_to_date;
  uint8_t view_up_to_date;
} say_target;

say_target *say_target_create();
void say_target_free(say_target *target);

void say_target_set_context_proc(say_target *target, say_context_proc proc);
void say_target_need_own_contxt(say_target *target, uint8_t val);
say_context *say_target_get_context(say_target *target);

void say_target_set_bind_hook(say_target *target, say_bind_hook proc);

void say_target_set_custom_data(say_target *target, void *data);

void say_target_set_size(say_target *target, say_vector2 size);
say_vector2 say_target_get_size(say_target *target);

void say_target_set_view(say_target *target, say_view *view);
say_view *say_target_get_view(say_target *target);
say_view *say_target_get_default_view(say_target *target);

say_shader *say_target_get_shader(say_target *target);

say_rect say_target_get_clip(say_target *target);
say_rect say_target_get_viewport_for(say_target *target, say_rect rect);

int say_target_make_current(say_target *target);

void say_target_clear(say_target *target, say_color color);
void say_target_draw(say_target *target, say_drawable *drawable);
void say_target_draw_buffer(say_target *target,
                            say_buffer_renderer *buf);

say_color say_target_get(say_target *target, size_t x, size_t y);
say_image *say_target_get_rect(say_target *target, size_t x, size_t y,
                               size_t w, size_t h);
say_image *say_target_to_image(say_target *target);

void say_target_update(say_target *target);

#endif
