#include "say.h"

static void say_view_update_matrix(say_view *view) {
  if (!view->custom_matrix) {
    say_matrix_reset(view->matrix);

    if (view->flip_y) {
      say_matrix_set_ortho(view->matrix,
                           view->center.x - view->size.x / 2,
                           view->center.x + view->size.x / 2,
                           view->center.y + view->size.y / 2,
                           view->center.y - view->size.y / 2,
                           -1, 1);
    }
    else {
      say_matrix_set_ortho(view->matrix,
                           view->center.x - view->size.x / 2,
                           view->center.x + view->size.x / 2,
                           view->center.y - view->size.y / 2,
                           view->center.y + view->size.y / 2,
                           -1, 1);
    }
  }

  view->matrix_updated = 1;
}

say_view *say_view_create() {
  say_view *view = (say_view*)malloc(sizeof(say_view));

  view->matrix = say_matrix_identity();
  view->matrix_updated = 0;
  view->custom_matrix  = 0;

  view->size   = say_make_vector2(2, 2);
  view->center = say_make_vector2(0, 0);

  view->viewport = say_make_rect(0, 0, 1, 1);

  view->flip_y = 1;

  return view;
}

void say_view_free(say_view *view) {
  say_matrix_free(view->matrix);
  free(view);
}

void say_view_copy(say_view *view, say_view *other) {
  say_matrix_set_content(view->matrix, other->matrix->content);

  view->matrix_updated = other->matrix_updated;
  view->custom_matrix  = other->custom_matrix;

  view->size   = other->size;
  view->center = other->center;

  view->viewport = other->viewport;
}

void say_view_zoom_by(say_view *view, say_vector2 scale) {
  say_view_set_size(view, say_make_vector2(view->size.x * scale.x,
                                           view->size.y * scale.y));
}

void say_view_set_size(say_view *view, say_vector2 size) {
  view->size = size;
  view->matrix_updated = 0;
  view->has_changed = 1;
}

void say_view_set_center(say_view *view, say_vector2 center) {
  view->center = center;
  view->matrix_updated = 0;
  view->has_changed = 1;
}

void say_view_set_viewport(say_view *view, say_rect viewport) {
  view->viewport = viewport;
  view->has_changed = 1;
}

say_vector2 say_view_get_size(say_view *view) {
  return view->size;
}

say_vector2 say_view_get_center(say_view *view) {
  return view->center;
}

say_rect say_view_get_viewport(say_view *view) {
  return view->viewport;
}


void say_view_flip_y(say_view *view, uint8_t val) {
  view->flip_y = val;
  view->matrix_updated = 0;
  view->has_changed = 1;
}

uint8_t say_view_is_y_flipped(say_view *view) {
  return view->flip_y;
}

say_matrix *say_view_get_matrix(say_view *view) {
  if (!view->matrix_updated)
    say_view_update_matrix(view);
  return view->matrix;
}

void say_view_set_matrix(say_view *view, say_matrix *matrix) {
  if (matrix) {
    view->custom_matrix = 1;
    say_matrix_set_content(view->matrix, matrix->content);
  }
  else {
    say_matrix_reset(view->matrix);
    view->custom_matrix = 0;
  }

  view->matrix_updated = 1;
  view->has_changed = 1;
}

uint8_t say_view_has_changed(say_view *view) {
  return view->has_changed;
}

void say_view_apply(say_view *view, say_shader *shader, say_vector2 size) {
  say_shader_set_matrix_id(shader, SAY_PROJECTION_LOC_ID,
                           say_view_get_matrix(view));

  glViewport(view->viewport.x * size.x,
             size.y - (view->viewport.y + view->viewport.h) * size.y,
             view->viewport.w * size.x,
             view->viewport.h * size.y);

  view->has_changed = 0;
}
