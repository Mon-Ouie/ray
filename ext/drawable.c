#include "ray.h"

VALUE ray_cDrawable = Qnil;

say_drawable *ray_rb2drawable(VALUE obj) {
  if (RAY_IS_A(obj, rb_path2class("Ray::Polygon")))
    return ray_rb2polygon(obj)->drawable;
  else if (RAY_IS_A(obj, rb_path2class("Ray::Sprite")))
    return ray_rb2sprite(obj)->drawable;
  else if (RAY_IS_A(obj, rb_path2class("Ray::Text")))
    return ray_rb2text(obj)->drawable;
  else {
    rb_raise(rb_eTypeError, "can't get a drawable pointer from a %s",
             RAY_OBJ_CLASSNAME(obj));
  }
}

/*
  The origin is used as the origin for translations, rotations, and
  scalings.

  @return [Ray::Vector2] val The origin.
*/
static
VALUE ray_drawable_origin(VALUE self) {
  return ray_vector2_to_rb(say_drawable_get_origin(ray_rb2drawable(self)));
}

/*
  @overload origin=(val)
    Sets the origin of the drawable.
    @see #origin
    @param [Ray::Vector2] The origin.
*/
static
VALUE ray_drawable_set_origin(VALUE self, VALUE val) {
  rb_check_frozen(self);
  say_drawable_set_origin(ray_rb2drawable(self), ray_convert_to_vector2(val));
  return val;
}

/*
  The scaling factor multiplies the size of the object. If it is set to (3,0.5),
  then te object is 3 times wider and 2 times shorter.

  @return [Ray::Vector2] The scaling factor.
*/
static
VALUE ray_drawable_scale(VALUE self) {
  return ray_vector2_to_rb(say_drawable_get_scale(ray_rb2drawable(self)));
}

/*
  @overload scale=(val)
    Sets the scaling factor of the drawable.
    @see #scale
    @param [Ray::Vector2] val The scaling factor
 */
VALUE ray_drawable_set_scale(VALUE self, VALUE val) {
  rb_check_frozen(self);
  say_drawable_set_scale(ray_rb2drawable(self), ray_convert_to_vector2(val));
  return val;
}

/*
  Position of the drawable. This is thus the translation applied to it.

  @return [Ray::Vector2] The position of the drawable
*/
static
VALUE ray_drawable_pos(VALUE self) {
  return ray_vector2_to_rb(say_drawable_get_pos(ray_rb2drawable(self)));
}

/*
  @overload pos=(val)
    Sets the position of the drawable.
    @see #pos
    @param [Ray::Vector2] val The new position of the drawable
*/
static
VALUE ray_drawable_set_pos(VALUE self, VALUE val) {
  rb_check_frozen(self);
  say_drawable_set_pos(ray_rb2drawable(self), ray_convert_to_vector2(val));
  return val;
}

/*
  The Z order is a number between 1 and -1. Numbers with a smaller Z order are
  drawn behind those with a higher one.

  Notice that this is only true without transparency. When transparency is
  enabled, transparent parts of a drawable would hide what's behind it if the
  scene isn't drawn in pixel order.

  @return [Float] The z order.
*/
static
VALUE ray_drawable_z(VALUE self) {
  return rb_float_new(say_drawable_get_z(ray_rb2drawable(self)));
}

/*
  @overload z=(val)
    Sets the z order.
    @see #z
    @param [Float] val The z order.
*/
static
VALUE ray_drawable_set_z(VALUE self, VALUE val) {
  rb_check_frozen(self);
  say_drawable_set_z(ray_rb2drawable(self), NUM2DBL(val));
  return val;
}

/*
  Angle is a rotation applied to a drawable. It is expressed in degrees, in the
  counter-clockwise direction.

  @return [Float] The rotation applied to the drawable
*/
static
VALUE ray_drawable_angle(VALUE self) {
  return rb_float_new(say_drawable_get_angle(ray_rb2drawable(self)));
}

/*
  @overload angle=(val)
    Sets the rotation applied to the drawable.
    @see #angle=
    @param [Float] val The rotation applied to the drawable.
*/
static
VALUE ray_drawable_set_angle(VALUE self, VALUE val) {
  rb_check_frozen(self);
  say_drawable_set_angle(ray_rb2drawable(self), NUM2DBL(val));
  return val;
}

/*
  @return [Ray::Matrix] The transformation matrix used by this object.
*/
static
VALUE ray_drawable_matrix(VALUE self) {
  return ray_matrix2rb(say_drawable_get_matrix(ray_rb2drawable(self)));
}

/*
  @overload transform(point)
    Applies the transformations to a point.

    @param [Ray::Vector3] point Point to transform.
    @return [Ray::Vector3] Transformed point
*/
static
VALUE ray_drawable_transform(VALUE self, VALUE point) {
  say_vector3 res = say_drawable_transform(ray_rb2drawable(self),
                                           ray_convert_to_vector3(point));
  return ray_vector3_to_rb(res);
}

/* @return [Ray::Shader] */
static
VALUE ray_drawable_shader(VALUE self) {
  return rb_iv_get(self, "@shader");
}

static
VALUE ray_drawable_set_shader(VALUE self, VALUE val) {
  if (NIL_P(val))
    say_drawable_set_shader(ray_rb2drawable(self), NULL);
  else
    say_drawable_set_shader(ray_rb2drawable(self), ray_rb2shader(val));

  rb_iv_set(self, "@shader", val);
  return val;
}

void Init_ray_drawable() {
  ray_cDrawable = rb_define_class_under(ray_mRay, "Drawable", rb_cObject);

  rb_define_method(ray_cDrawable, "origin", ray_drawable_origin, 0);
  rb_define_method(ray_cDrawable, "origin=", ray_drawable_set_origin, 1);

  rb_define_method(ray_cDrawable, "scale", ray_drawable_scale, 0);
  rb_define_method(ray_cDrawable, "scale=", ray_drawable_set_scale, 1);

  rb_define_method(ray_cDrawable, "pos", ray_drawable_pos, 0);
  rb_define_method(ray_cDrawable, "pos=", ray_drawable_set_pos, 1);

  rb_define_method(ray_cDrawable, "z", ray_drawable_z, 0);
  rb_define_method(ray_cDrawable, "z=", ray_drawable_set_z, 1);

  rb_define_method(ray_cDrawable, "angle", ray_drawable_angle, 0);
  rb_define_method(ray_cDrawable, "angle=", ray_drawable_set_angle, 1);

  rb_define_method(ray_cDrawable, "matrix", ray_drawable_matrix, 0);
  rb_define_method(ray_cDrawable, "transform", ray_drawable_transform, 1);

  rb_define_method(ray_cDrawable, "shader", ray_drawable_shader, 0);
  rb_define_method(ray_cDrawable, "shader=", ray_drawable_set_shader, 1);
}
