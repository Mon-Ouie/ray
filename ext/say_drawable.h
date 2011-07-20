#ifndef SAY_DRAWABLE_H_
#define SAY_DRAWABLE_H_

#include "say_buffer_slice.h"
#include "say_index_buffer_slice.h"

#include "say_matrix.h"
#include "say_shader.h"

typedef void (*say_fill_proc)(void *data, void *vertices);
typedef void (*say_index_fill_proc)(void *data, GLuint *indices, size_t from);
typedef void (*say_render_proc)(void *data, size_t first, size_t index);
typedef void (*say_shader_proc)(void *data, say_shader *shader);

typedef struct {
  size_t            vertex_count;
  size_t            vtype;
  say_buffer_slice *slice;

  size_t                  index_count;
  say_index_buffer_slice *index_slice;

  void *data;
  void *other_data;

  say_fill_proc       fill_proc;
  say_index_fill_proc index_fill_proc;
  say_render_proc     render_proc;
  say_shader_proc     shader_proc;

  say_shader *shader;
  say_matrix *matrix;

  say_vector2 origin;
  say_vector2 scale;
  say_vector2 pos;
  float       z_order;
  float       angle;

  bool use_texture;
  bool matrix_updated;
  bool custom_matrix;
  bool has_changed;
} say_drawable;

say_drawable *say_drawable_create(size_t vtype);
void say_drawable_free(say_drawable *drawable);

void say_drawable_copy(say_drawable *drawable, say_drawable *other);

void say_drawable_set_custom_data(say_drawable *drawable, void *data);
void say_drawable_set_other_data(say_drawable *drawable, void *data);
void *say_drawable_get_other_data(say_drawable *drawable);

void say_drawable_set_vertex_count(say_drawable *drawable, size_t size);
size_t say_drawable_get_vertex_count(say_drawable *drawable);

size_t say_drawable_get_vertex_type(say_drawable *drawable);

void say_drawable_set_index_count(say_drawable *drawable, size_t size);
size_t say_drawable_get_index_count(say_drawable *drawable);

void say_drawable_set_fill_proc(say_drawable *drawable, say_fill_proc proc);
void say_drawable_set_render_proc(say_drawable *drawable, say_render_proc proc);
void say_drawable_set_shader_proc(say_drawable *drawable, say_shader_proc proc);
void say_drawable_set_index_fill_proc(say_drawable *drawable,
                                        say_index_fill_proc proc);

void say_drawable_fill_buffer(say_drawable *drawable, void *vertices);
void say_drawable_fill_own_buffer(say_drawable *drawable);

void say_drawable_fill_index_buffer(say_drawable *drawable, GLuint *indices,
                                    size_t from);
void say_drawable_fill_own_index_buffer(say_drawable *drawable);

void say_drawable_draw_at(say_drawable *drawable, size_t vertex, size_t id,
                          say_shader *shader);
void say_drawable_draw(say_drawable *drawable, say_shader *shader);

void say_drawable_set_changed(say_drawable *drawable);
uint8_t say_drawable_has_changed(say_drawable *drawable);

void say_drawable_set_textured(say_drawable *drawable, uint8_t val);
uint8_t say_drawable_is_textured(say_drawable *drawable);

say_shader *say_drawable_get_shader(say_drawable *drawable);
void say_drawable_set_shader(say_drawable *drawable, say_shader *shader);

void say_drawable_set_origin(say_drawable *drawable, say_vector2 origin);
void say_drawable_set_scale(say_drawable *drawable, say_vector2 scale);
void say_drawable_set_pos(say_drawable *drawable, say_vector2 pos);
void say_drawable_set_z(say_drawable *drawable, float z);
void say_drawable_set_angle(say_drawable *drawable, float angle);

say_vector2 say_drawable_get_origin(say_drawable *drawable);
say_vector2 say_drawable_get_scale(say_drawable *drawable);
say_vector2 say_drawable_get_pos(say_drawable *drawable);
float say_drawable_get_z(say_drawable *drawable);
float say_drawable_get_angle(say_drawable *drawable);

say_matrix *say_drawable_get_matrix(say_drawable *drawable);
void say_drawable_set_matrix(say_drawable *drawable, say_matrix *matrix);
say_vector3 say_drawable_transform(say_drawable *drawable, say_vector3 point);

#endif
