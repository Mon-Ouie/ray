#include "ray.h"

VALUE ray_cRect = Qnil;

say_rect *ray_rb2rect_ptr(VALUE obj) {
  if (!RAY_IS_A(obj, rb_path2class("Ray::Rect"))) {
    rb_raise(rb_eTypeError, "Can't convert %s into Ray::Rect",
             RAY_OBJ_CLASSNAME(obj));
  }

  say_rect *rect;
  Data_Get_Struct(obj, say_rect, rect);

  return rect;
}

say_rect ray_rb2rect(VALUE obj) {
  return *ray_rb2rect_ptr(obj);
}

say_rect ray_convert_to_rect(VALUE obj) {
  obj = rb_funcall(obj, RAY_METH("to_rect"), 0);
  return ray_rb2rect(obj);
}

VALUE ray_rect2rb(say_rect rect) {
  say_rect *obj = malloc(sizeof(say_rect));
  *obj = rect;

  return Data_Wrap_Struct(ray_cRect, 0, free, obj);
}

static
VALUE ray_alloc_rect(VALUE self) {
  say_rect *rect = malloc(sizeof(say_rect));
  *rect = say_make_rect(0, 0, 0, 0);

  return Data_Wrap_Struct(self, 0, free, rect);
}

/*
 * @overload initialize(x, y)
 *   Creates a new rect with size set to (0, 0).
 *
 * @overload initialize(x, y, w, h)
 *   Creates a new rect with the specified size.
 */
static
VALUE ray_init_rect(int argc, VALUE *argv, VALUE self) {
  VALUE x, y, w, h;
  rb_scan_args(argc, argv, "22", &x, &y, &w, &h);

  say_rect *rect;
  Data_Get_Struct(self, say_rect, rect);

  rect->x = NUM2DBL(x);
  rect->y = NUM2DBL(y);

  if (!NIL_P(w)) {
    rect->w = NUM2DBL(w);
    rect->h = NUM2DBL(h);
  }
  else {
    rect->w = 0;
    rect->h = 0;
  }

  return Qnil;
}

static
VALUE ray_init_rect_copy(VALUE self, VALUE other) {
  say_rect *rect = NULL, *source = NULL;
  Data_Get_Struct(self,  say_rect, rect);
  Data_Get_Struct(other, say_rect, source);

  *rect = *source;
  return self;
}

/* @return [Float] X position of the rect */
static
VALUE ray_rect_x(VALUE self) {
  say_rect *rect;
  Data_Get_Struct(self, say_rect, rect);

  return rb_float_new(rect->x);
}

/* @return [Float] Y position of the rect */
static
VALUE ray_rect_y(VALUE self) {
  say_rect *rect;
  Data_Get_Struct(self, say_rect, rect);

  return rb_float_new(rect->y);
}

/* @return [Float] width of the rect */
static
VALUE ray_rect_w(VALUE self) {
  say_rect *rect;
  Data_Get_Struct(self, say_rect, rect);

  return rb_float_new(rect->w);
}

/* @return [Float] height of the rect */
static
VALUE ray_rect_h(VALUE self) {
  say_rect *rect;
  Data_Get_Struct(self, say_rect, rect);

  return rb_float_new(rect->h);
}

/*
  @overload x=(val)
    Sets the x position of the rect

    @param [Float] val The new x position of the rect
    @return [void]
*/
static
VALUE ray_rect_set_x(VALUE self, VALUE val) {
  rb_check_frozen(self);

  say_rect *rect;
  Data_Get_Struct(self, say_rect, rect);

  rect->x = NUM2DBL(val);

  return val;
}

/*
  @overload y=(val)
    Changes the y position of the rect.

    @param [Float] val The new y position of the rect.
    @return [void]
*/
static
VALUE ray_rect_set_y(VALUE self, VALUE val) {
  rb_check_frozen(self);

  say_rect *rect;
  Data_Get_Struct(self, say_rect, rect);

  rect->y = NUM2DBL(val);

  return val;
}

/*
  @overload w=(val)
    Changes the with of the rect

    @param [Float] val The new width of the rect.
    @return [void]
*/
static
VALUE ray_rect_set_w(VALUE self, VALUE val) {
  rb_check_frozen(self);

  say_rect *rect;
  Data_Get_Struct(self, say_rect, rect);

  rect->w = NUM2DBL(val);

  return val;
}

/*
  @overload h=(val)
    Sets the height of the rect

    @param [Float] val The new height of the rect.
    @return [void]
*/
static
VALUE ray_rect_set_h(VALUE self, VALUE val) {
  rb_check_frozen(self);

  say_rect *rect;
  Data_Get_Struct(self, say_rect, rect);

  rect->h = NUM2DBL(val);

  return val;
}

/*
  Document-class: Ray::Rect

  Rects are used to represent a part of an image, using
  two attributes to represent its position, and two
  others to represent its size.
*/

void Init_ray_rect() {
  ray_cRect = rb_define_class_under(ray_mRay, "Rect", rb_cObject);

  rb_define_alloc_func(ray_cRect, ray_alloc_rect);
  rb_define_method(ray_cRect, "initialize", ray_init_rect, -1);
  rb_define_method(ray_cRect, "initialize_copy", ray_init_rect_copy, 1);

  rb_define_method(ray_cRect, "x", ray_rect_x, 0);
  rb_define_method(ray_cRect, "y", ray_rect_y, 0);
  rb_define_method(ray_cRect, "width", ray_rect_w, 0);
  rb_define_method(ray_cRect, "height", ray_rect_h, 0);

  rb_define_method(ray_cRect, "x=", ray_rect_set_x, 1);
  rb_define_method(ray_cRect, "y=", ray_rect_set_y, 1);
  rb_define_method(ray_cRect, "width=", ray_rect_set_w, 1);
  rb_define_method(ray_cRect, "height=", ray_rect_set_h, 1);
}
