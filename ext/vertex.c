#include "ray.h"

VALUE ray_cVertex = Qnil;

say_vertex *ray_rb2vertex(VALUE obj) {
  if (!rb_obj_is_kind_of(obj, rb_path2class("Ray::Vertex"))) {
    rb_raise(rb_eTypeError, "can't convert %s into Ray::Vertex",
             RAY_OBJ_CLASSNAME(obj));
  }

  say_vertex *ptr = NULL;
  Data_Get_Struct(obj, say_vertex, ptr);

  return ptr;
}

static
VALUE ray_vertex_alloc(VALUE self) {
  say_vertex *vertex = malloc(sizeof(say_vertex));
  return Data_Wrap_Struct(self, NULL, free, vertex);
}

/*
  @overload initialize(pos = [0, 0], color = Color.white, tex = [0, 0])
    @param [Ray::Vector2] pos Position of the vertex
    @param [Ray::Color] color Color of the vertex
    @param [Ray::Vector2] tex Texture position of the vertex
*/
static
VALUE ray_vertex_init(int argc, VALUE *argv, VALUE self) {
  say_vertex *vertex = ray_rb2vertex(self);

  VALUE pos, col, tex;
  rb_scan_args(argc, argv, "03", &pos, &col, &tex);

  vertex->pos = NIL_P(pos) ? say_make_vector2(0, 0) : ray_convert_to_vector2(pos);
  vertex->col = NIL_P(col) ? say_make_color(255, 255, 255, 255) : ray_rb2col(col);
  vertex->tex = NIL_P(tex) ? say_make_vector2(0, 0) : ray_convert_to_vector2(tex);

  return self;
}

/* @return [Ray::Vector2] Position of the vertex */
static
VALUE ray_vertex_pos(VALUE self) {
  return ray_vector2_to_rb(ray_rb2vertex(self)->pos);
}

static
VALUE ray_vertex_set_pos(VALUE self, VALUE val) {
  rb_check_frozen(self);
  ray_rb2vertex(self)->pos = ray_convert_to_vector2(val);
  return val;
}

/* @return [Ray::Vector2] Texture position of the vertex */
static
VALUE ray_vertex_tex(VALUE self) {
  return ray_vector2_to_rb(ray_rb2vertex(self)->tex);
}

static
VALUE ray_vertex_set_tex(VALUE self, VALUE val) {
  rb_check_frozen(self);
  ray_rb2vertex(self)->tex = ray_convert_to_vector2(val);
  return val;
}

/* @return [Ray::Color] Color of the vertex */
static
VALUE ray_vertex_col(VALUE self) {
  return ray_col2rb(ray_rb2vertex(self)->col);
}

static
VALUE ray_vertex_set_col(VALUE self, VALUE val) {
  rb_check_frozen(self);
  ray_rb2vertex(self)->col = ray_rb2col(val);
  return val;
}

void Init_ray_vertex() {
  ray_cVertex = rb_define_class_under(ray_mRay, "Vertex", rb_cObject);
  rb_define_alloc_func(ray_cVertex, ray_vertex_alloc);
  rb_define_method(ray_cVertex, "initialize", ray_vertex_init, -1);

  rb_define_method(ray_cVertex, "pos", ray_vertex_pos, 0);
  rb_define_method(ray_cVertex, "tex", ray_vertex_tex, 0);
  rb_define_method(ray_cVertex, "col", ray_vertex_col, 0);

  rb_define_method(ray_cVertex, "pos=", ray_vertex_set_pos, 1);
  rb_define_method(ray_cVertex, "tex=", ray_vertex_set_tex, 1);
  rb_define_method(ray_cVertex, "col=", ray_vertex_set_col, 1);
}
