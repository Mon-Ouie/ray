#ifndef SAY_VIEW_H_
#define SAY_VIEW_H_

#include "say_matrix.h"

typedef struct {
  say_matrix *matrix;
  uint8_t matrix_updated;
  uint8_t custom_matrix;
  uint8_t has_changed;

  say_vector2 size;
  say_vector2 center;

  say_rect viewport;
  uint8_t viewport_changed;

  uint8_t flip_y;
} say_view;

say_view *say_view_create();
void say_view_free(say_view *view);
void say_view_copy(say_view *view, say_view *other);

void say_view_zoom_by(say_view *view, say_vector2 scale);

void say_view_set_size(say_view *view, say_vector2 size);
void say_view_set_center(say_view *view, say_vector2 center);
void say_view_set_viewport(say_view *view, say_rect viewport);

say_vector2 say_view_get_size(say_view *view);
say_vector2 say_view_get_center(say_view *view);
say_rect say_view_get_viewport(say_view *view);

void say_view_flip_y(say_view *view, uint8_t val);
uint8_t say_view_is_y_flipped(say_view *view);

say_matrix *say_view_get_matrix(say_view *view);
void say_view_set_matrix(say_view *view, say_matrix *matrix);

uint8_t say_view_has_changed(say_view *view);
void say_view_apply(say_view *view, say_shader *shader, say_vector2 size);

#endif
