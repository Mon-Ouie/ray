#include "ray.h"

VALUE ray_cColor = Qnil;

#define ray_color_clamp(col) ray_byte_clamp(col)

say_color ray_rb2col(VALUE object) {
  if (!RAY_IS_A(object, rb_path2class("Ray::Color"))) {
    rb_raise(rb_eTypeError, "Can't convert %s into Ray::Color",
             RAY_OBJ_CLASSNAME(object));
  }

  say_color *ret = NULL;
  Data_Get_Struct(object, say_color, ret);

  return *ret;
}

VALUE ray_col2rb(say_color color) {
  say_color *ptr = malloc(sizeof(say_color));
  *ptr = color;
  return Data_Wrap_Struct(rb_path2class("Ray::Color"), 0, free, ptr);
}

static
VALUE ray_color_alloc(VALUE self) {
  say_color *ptr = malloc(sizeof(say_color));
  return Data_Wrap_Struct(self, 0, free, ptr);
}

/*
 * @overload initialize(red, green, blue, alpha = 255)
 *   Creates a new color. All the parameters must be integers between
 *     0 and 255. Alpha is the transparency (255: opaque, 0: invisible).
 */
static
VALUE ray_color_init(int argc, VALUE *argv, VALUE self) {
  VALUE r, g, b, a;
  rb_scan_args(argc, argv, "31", &r, &g, &b, &a);
  if (a == Qnil) a = INT2FIX(255);

  say_color *ret = NULL;
  Data_Get_Struct(self, say_color, ret);

  ret->r = ray_color_clamp(NUM2INT(r));
  ret->g = ray_color_clamp(NUM2INT(g));
  ret->b = ray_color_clamp(NUM2INT(b));
  ret->a = ray_color_clamp(NUM2INT(a));

  return Qnil;
}

static
VALUE ray_color_init_copy(VALUE self, VALUE other) {
  say_color *color = NULL, *source = NULL;
  Data_Get_Struct(self,  say_color, color);
  Data_Get_Struct(other, say_color, source);

  *color = *source;

  return self;
}

/* @return [Integer] Red intensity. */
static
VALUE ray_color_r(VALUE self) {
  say_color *ret;
  Data_Get_Struct(self, say_color, ret);

  return INT2FIX(ret->r);
}

/* @return [Integer] Green intensity. */
static
VALUE ray_color_g(VALUE self) {
  say_color *ret;
  Data_Get_Struct(self, say_color, ret);

  return INT2FIX(ret->g);
}

/* @return [Integer] Blue intensity. */
static
VALUE ray_color_b(VALUE self) {
  say_color *ret;
  Data_Get_Struct(self, say_color, ret);

  return INT2FIX(ret->b);
}

/* @return [Integer] Alpha opacity. */
static
VALUE ray_color_a(VALUE self) {
  say_color *ret;
  Data_Get_Struct(self, say_color, ret);

  return INT2FIX(ret->a);
}

/*
 *  @overload r=(val)
 *    Sets the red intensity
 *    @param [Integer] The new red intensity.
 */
static
VALUE ray_color_set_r(VALUE self, VALUE val) {
  rb_check_frozen(self);

  say_color *ret;
  Data_Get_Struct(self, say_color, ret);

  ret->r = ray_color_clamp(NUM2INT(val));
  return val;
}

/*
 * @overload g=(val)
 *   Sets the green intensity
 *   @param [Integer] The new green intensity.
*/
static
VALUE ray_color_set_g(VALUE self, VALUE val) {
  rb_check_frozen(self);

  say_color *ret;
  Data_Get_Struct(self, say_color, ret);

  ret->g = ray_color_clamp(NUM2INT(val));
  return val;
}

/*
 * @overload blue=(val)
 *   Sets the blue intensity
 *   @param [Integer] The new blue intensity.
 */
static
VALUE ray_color_set_b(VALUE self, VALUE val) {
  rb_check_frozen(self);

  say_color *ret;
  Data_Get_Struct(self, say_color, ret);

  ret->b = ray_color_clamp(NUM2INT(val));
  return val;
}

/*
 * @overload a=(val)
 *   Sets the alpha opacity
 *   @param [Integer] The new alpha opacity.
 */
static
VALUE ray_color_set_a(VALUE self, VALUE val) {
  rb_check_frozen(self);

  say_color *ret;
  Data_Get_Struct(self, say_color, ret);

  ret->a = ray_color_clamp(NUM2INT(val));
  return val;
}

void Init_ray_color() {
  ray_cColor = rb_define_class_under(ray_mRay, "Color", rb_cObject);
  rb_define_alloc_func(ray_cColor, ray_color_alloc);
  rb_define_method(ray_cColor, "initialize", ray_color_init, -1);
  rb_define_method(ray_cColor, "initialize_copy", ray_color_init_copy, 1);

  rb_define_method(ray_cColor, "r", ray_color_r, 0);
  rb_define_method(ray_cColor, "g", ray_color_g, 0);
  rb_define_method(ray_cColor, "b", ray_color_b, 0);
  rb_define_method(ray_cColor, "a", ray_color_a, 0);

  rb_define_method(ray_cColor, "r=", ray_color_set_r, 1);
  rb_define_method(ray_cColor, "g=", ray_color_set_g, 1);
  rb_define_method(ray_cColor, "b=", ray_color_set_b, 1);
  rb_define_method(ray_cColor, "a=", ray_color_set_a, 1);
}
