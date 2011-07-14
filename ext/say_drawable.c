#include "say.h"

static void say_drawable_update_matrix(say_drawable *drawable) {
  if (drawable->custom_matrix)
    return;

  say_matrix_reset(drawable->matrix);

  say_matrix_translate_by(drawable->matrix,
                          drawable->pos.x,
                          drawable->pos.y,
                          drawable->z_order);
  say_matrix_rotate(drawable->matrix, drawable->angle, 0, 0, 1);
  say_matrix_scale_by(drawable->matrix, drawable->scale.x, drawable->scale.y,
                      1);
  say_matrix_translate_by(drawable->matrix,
                          -drawable->origin.x,
                          -drawable->origin.y,
                          0);

  drawable->matrix_updated = 1;
}

say_drawable *say_drawable_create(size_t vtype) {
  say_drawable *drawable = (say_drawable*)malloc(sizeof(say_drawable));

  drawable->vertex_count = 0;
  drawable->vtype        = vtype;
  drawable->slice        = NULL;

  drawable->data = NULL;

  drawable->fill_proc   = NULL;
  drawable->render_proc = NULL;

  drawable->shader = NULL;
  drawable->matrix = say_matrix_identity();

  drawable->matrix_updated = false;
  drawable->custom_matrix  = false;
  drawable->use_texture    = false;
  drawable->has_changed    = true;

  drawable->origin  = say_make_vector2(0, 0);
  drawable->scale   = say_make_vector2(1, 1);
  drawable->pos     = say_make_vector2(0, 0);
  drawable->angle   = 0;
  drawable->z_order = 0;

  return drawable;
}

void say_drawable_copy(say_drawable *drawable, say_drawable *other) {
  drawable->vertex_count = other->vertex_count;
  drawable->vtype        = other->vtype;

  drawable->data = other->data;

  drawable->fill_proc   = other->fill_proc;
  drawable->render_proc = other->render_proc;

  drawable->shader = other->shader;

  drawable->origin  = other->origin;
  drawable->scale   = other->scale;
  drawable->pos     = other->pos;
  drawable->z_order = other->z_order;
  drawable->angle   = other->angle;

  drawable->use_texture = other->use_texture;

  drawable->matrix_updated = false;
  drawable->has_changed    = true;
}

void say_drawable_free(say_drawable *drawable) {
  if (drawable->slice)
    say_buffer_slice_free(drawable->slice);
  say_matrix_free(drawable->matrix);
  free(drawable);
}

void say_drawable_set_custom_data(say_drawable *drawable, void *data) {
  if (data == drawable->data)
    return;

  drawable->data        = data;
  drawable->has_changed = 1;
}

void say_drawable_set_vertex_count(say_drawable *drawable, size_t size) {
  if (size == drawable->vertex_count)
    return;

  drawable->vertex_count = size;
  drawable->has_changed  = 1;
}

size_t say_drawable_get_vertex_count(say_drawable *drawable) {
  return drawable->vertex_count;
}

size_t say_drawable_get_vertex_type(say_drawable *drawable) {
  return drawable->vtype;
}

void say_drawable_set_fill_proc(say_drawable *drawable, say_fill_proc proc) {
  drawable->fill_proc   = proc;
  drawable->has_changed = 1;
}

void say_drawable_set_render_proc(say_drawable *drawable, say_render_proc proc) {
  drawable->render_proc = proc;
  drawable->has_changed = 1;
}

void say_drawable_fill_buffer(say_drawable *drawable, void *vertices) {
  if (drawable->fill_proc)
    drawable->fill_proc(drawable->data, vertices);
}

void say_drawable_fill_own_buffer(say_drawable *drawable) {
  if (drawable->has_changed) {
    if (!drawable->slice)
      drawable->slice = say_buffer_slice_create(drawable->vtype,
                                                drawable->vertex_count);

    if (say_buffer_slice_get_size(drawable->slice) != drawable->vertex_count) {
      say_buffer_slice_recreate(drawable->slice, drawable->vertex_count);
    }

    if (drawable->fill_proc) {
      drawable->fill_proc(drawable->data,
                          say_buffer_slice_get_vertex(drawable->slice, 0));
    }

    drawable->has_changed = 0;
    say_buffer_slice_update(drawable->slice);
  }
}

void say_drawable_draw_at(say_drawable *drawable, size_t id,
                          say_shader *shader) {
  if (!drawable->matrix_updated)
    say_drawable_update_matrix(drawable);

  say_shader *used_shader = drawable->shader ? drawable->shader : shader;
  say_shader_set_matrix_id(used_shader, SAY_MODEL_VIEW_LOC_ID,
                           drawable->matrix);

  if (drawable->render_proc) {
    if (drawable->shader) {
      say_shader_set_int_id(drawable->shader, SAY_TEXTURE_ENABLED_LOC_ID,
                            drawable->use_texture);
    }

    drawable->render_proc(drawable->data, id, shader);
  }
}

void say_drawable_draw(say_drawable *drawable, say_shader *shader) {
  say_drawable_fill_own_buffer(drawable);

  if (!drawable->matrix_updated)
    say_drawable_update_matrix(drawable);

  /* NB: the current shader is always bound because we set a variable in it. */
  say_shader *used_shader = drawable->shader ? drawable->shader : shader;
  say_shader_set_matrix_id(used_shader, SAY_MODEL_VIEW_LOC_ID,
                           drawable->matrix);

  if (drawable->render_proc) {
    if (drawable->shader) {
      say_shader_set_int_id(drawable->shader, SAY_TEXTURE_ENABLED_LOC_ID,
                            drawable->use_texture);
    }

    say_buffer_slice_bind(drawable->slice);

    drawable->render_proc(drawable->data,
                          say_buffer_slice_get_loc(drawable->slice),
                          used_shader);
  }
}

void say_drawable_set_changed(say_drawable *drawable) {
  drawable->has_changed = 1;
}

uint8_t say_drawable_has_changed(say_drawable *drawable) {
  return drawable->has_changed;
}

void say_drawable_set_textured(say_drawable *drawable, uint8_t val) {
  drawable->use_texture = val;
}

uint8_t say_drawable_is_textured(say_drawable *drawable) {
  return drawable->use_texture;
}

say_shader *say_drawable_get_shader(say_drawable *drawable) {
  return drawable->shader;
}

void say_drawable_set_shader(say_drawable *drawable, say_shader *shader) {
  drawable->shader = shader;
}

void say_drawable_set_origin(say_drawable *drawable, say_vector2 origin) {
  drawable->matrix_updated = 0;
  drawable->origin = origin;
}

void say_drawable_set_scale(say_drawable *drawable, say_vector2 scale) {
  drawable->matrix_updated = 0;
  drawable->scale = scale;
}

void say_drawable_set_pos(say_drawable *drawable, say_vector2 pos) {
  drawable->matrix_updated = 0;
  drawable->pos = pos;
}

void say_drawable_set_z(say_drawable *drawable, float z) {
  drawable->matrix_updated = 0;
  drawable->z_order = z;
}

void say_drawable_set_angle(say_drawable *drawable, float angle) {
  drawable->matrix_updated = 0;
  drawable->angle = angle;
}

say_vector2 say_drawable_get_origin(say_drawable *drawable) {
  return drawable->origin;
}

say_vector2 say_drawable_get_scale(say_drawable *drawable) {
  return drawable->scale;
}

say_vector2 say_drawable_get_pos(say_drawable *drawable) {
  return drawable->pos;
}

float say_drawable_get_z(say_drawable *drawable) {
  return drawable->z_order;
}

float say_drawable_get_angle(say_drawable *drawable) {
  return drawable->angle;
}

say_matrix *say_drawable_get_matrix(say_drawable *drawable) {
  if (!drawable->matrix_updated)
    say_drawable_update_matrix(drawable);

  return drawable->matrix;
}

void say_drawable_set_matrix(say_drawable *drawable, say_matrix *matrix) {
  if (matrix) {
    drawable->custom_matrix = true;
    memcpy(drawable->matrix->content, matrix->content, sizeof(float) * 16);
  }
  else {
    drawable->custom_matrix  = false;
    drawable->matrix_updated = false;
  }
}

say_vector3 say_drawable_transform(say_drawable *drawable, say_vector3 point) {
  if (!drawable->matrix_updated)
    say_drawable_update_matrix(drawable);

  return say_matrix_transform(drawable->matrix, point);
}
