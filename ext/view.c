#include "ray.h"

VALUE ray_cView = Qnil;

VALUE ray_view2rb(say_view *view) {
  say_view *cpy = say_view_create();
  say_view_copy(cpy, view);
  return Data_Wrap_Struct(rb_path2class("Ray::View"), NULL, say_view_free, cpy);
}

say_view *ray_rb2view(VALUE obj) {
  if (!RAY_IS_A(obj, rb_path2class("Ray::View"))) {
    rb_raise(rb_eTypeError, "can't convert %s into Ray::View",
             RAY_OBJ_CLASSNAME(obj));
  }

  say_view *view = NULL;
  Data_Get_Struct(obj, say_view, view);

  return view;
}

static
VALUE ray_view_alloc(VALUE self) {
  say_view *view = say_view_create();
  return Data_Wrap_Struct(self, NULL, say_view_free, view);
}

/*
  @overload initialize(center, size, viewport = [0, 0, 1, 1])
    @param [Ray::Vector2] center Center of the view
    @param [Ray::Vector2] size Size of the view

    @param [Ray::Rect] rect The rect this view occupies. It
  @overload initialize(matrix)
    @param [Ray::Matrix] matrix Model view matrix
*/
static
VALUE ray_view_init(int argc, VALUE *argv, VALUE self) {
  say_view *view = ray_rb2view(self);

  if (argc == 1) {
    say_view_set_matrix(view, ray_rb2matrix(argv[0]));
  }
  else {
    VALUE center, size, viewport = Qnil;
    rb_scan_args(argc, argv, "21", &center, &size, &viewport);

    say_view_set_center(view, ray_convert_to_vector2(center));
    say_view_set_size(view, ray_convert_to_vector2(size));

    if (!NIL_P(viewport)) {
      say_view_set_viewport(view, ray_convert_to_rect(viewport));
    }
  }

  return self;
}

static
VALUE ray_view_init_copy(VALUE self, VALUE orig) {
  say_view_copy(ray_rb2view(self), ray_rb2view(orig));
  return self;
}

/*
  @overload zoom_by(vector)
    @param [Vector2] vector Multiplies the vector size by vector
 */
VALUE ray_view_zoom_by(VALUE self, VALUE val) {
  rb_check_frozen(self);
  say_view_zoom_by(ray_rb2view(self), ray_convert_to_vector2(val));
  return self;
}

static
VALUE ray_view_size(VALUE self) {
  return ray_vector2_to_rb(say_view_get_size(ray_rb2view(self)));
}

static
VALUE ray_view_set_size(VALUE self, VALUE val) {
  rb_check_frozen(self);
  say_view_set_size(ray_rb2view(self), ray_convert_to_vector2(val));
  return val;
}

static
VALUE ray_view_center(VALUE self) {
  return ray_vector2_to_rb(say_view_get_center(ray_rb2view(self)));
}

static
VALUE ray_view_set_center(VALUE self, VALUE val) {
  rb_check_frozen(self);
  say_view_set_center(ray_rb2view(self), ray_convert_to_vector2(val));
  return val;
}

static
VALUE ray_view_viewport(VALUE self) {
  return ray_rect2rb(say_view_get_viewport(ray_rb2view(self)));
}

static
VALUE ray_view_set_viewport(VALUE self, VALUE val) {
  rb_check_frozen(self);
  say_view_set_viewport(ray_rb2view(self), ray_convert_to_rect(val));
  return val;
}

static
VALUE ray_view_matrix(VALUE self) {
  return ray_matrix2rb(say_view_get_matrix(ray_rb2view(self)));
}

static
VALUE ray_view_set_matrix(VALUE self, VALUE val) {
  rb_check_frozen(self);
  say_view_set_matrix(ray_rb2view(self), ray_rb2matrix(val));
  return val;
}

void Init_ray_view() {
  ray_cView = rb_define_class_under(ray_mRay, "View", rb_cObject);
  rb_define_alloc_func(ray_cView, ray_view_alloc);
  rb_define_method(ray_cView, "initialize", ray_view_init, -1);
  rb_define_method(ray_cView, "initialize_copy", ray_view_init_copy, 1);

  rb_define_method(ray_cView, "zoom_by", ray_view_zoom_by, 1);

  rb_define_method(ray_cView, "size", ray_view_size, 0);
  rb_define_method(ray_cView, "size=", ray_view_set_size, 1);

  rb_define_method(ray_cView, "center", ray_view_center, 0);
  rb_define_method(ray_cView, "center=", ray_view_set_center, 1);

  rb_define_method(ray_cView, "viewport", ray_view_viewport, 0);
  rb_define_method(ray_cView, "viewport=", ray_view_set_viewport, 1);

  rb_define_method(ray_cView, "matrix", ray_view_matrix, 0);
  rb_define_method(ray_cView, "matrix=", ray_view_set_matrix, 1);
}
