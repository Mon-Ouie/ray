#include "ray.h"

VALUE ray_cVector2 = Qnil;
VALUE ray_cVector3 = Qnil;

say_vector2 *ray_rb2vector2_ptr(VALUE obj) {
  if (!RAY_IS_A(obj, rb_path2class("Ray::Vector2"))) {
    rb_raise(rb_eTypeError, "Can't convert %s into Ray::Vector2",
             RAY_OBJ_CLASSNAME(obj));
  }

  say_vector2 *vector = NULL;
  Data_Get_Struct(obj, say_vector2, vector);

  return vector;
}

say_vector2 ray_rb2vector2(VALUE obj) {
  return *ray_rb2vector2_ptr(obj);
}

say_vector2 ray_convert_to_vector2(VALUE obj) {
  obj = rb_funcall(obj, RAY_METH("to_vector2"), 0);
  return *ray_rb2vector2_ptr(obj);
}

VALUE ray_vector2_to_rb(say_vector2 vector) {
  say_vector2 *obj = malloc(sizeof(say_vector2));
  *obj = vector;
  return Data_Wrap_Struct(rb_path2class("Ray::Vector2"), NULL, free, obj);
}

say_vector3 *ray_rb2vector3_ptr(VALUE obj) {
  if (!RAY_IS_A(obj, rb_path2class("Ray::Vector3"))) {
    rb_raise(rb_eTypeError, "Can't convert %s into Ray::Vector3",
             RAY_OBJ_CLASSNAME(obj));
  }

  say_vector3 *vector = NULL;
  Data_Get_Struct(obj, say_vector3, vector);

  return vector;
}

say_vector3 ray_rb2vector3(VALUE obj) {
  return *ray_rb2vector3_ptr(obj);
}

say_vector3 ray_convert_to_vector3(VALUE obj) {
  obj = rb_funcall(obj, RAY_METH("to_vector3"), 0);
  return *ray_rb2vector3_ptr(obj);
}

VALUE ray_vector3_to_rb(say_vector3 vector) {
  say_vector3 *obj = malloc(sizeof(say_vector3));
  *obj = vector;
  return Data_Wrap_Struct(rb_path2class("Ray::Vector3"), NULL, free, obj);
}

static
VALUE ray_vector2_alloc(VALUE self) {
  say_vector2 *obj = malloc(sizeof(say_vector2));
  *obj = say_make_vector2(0, 0);

  return Data_Wrap_Struct(self, NULL, free, obj);
}

/*
  @overload initialize(x = 0.0, y = 0.0)
    @param [Float] x X position of the vector
    @param [Float] y Y postiion of the vector
*/
static
VALUE ray_vector2_init(int argc, VALUE *argv, VALUE self) {
  VALUE rb_x = Qnil, rb_y = Qnil;
  rb_scan_args(argc, argv, "02", &rb_x, &rb_y);

  say_vector2 *vector = ray_rb2vector2_ptr(self);
  if (!NIL_P(rb_x)) vector->x = NUM2DBL(rb_x);
  if (!NIL_P(rb_y)) vector->y = NUM2DBL(rb_y);

  return self;
}

static
VALUE ray_vector2_init_copy(VALUE self, VALUE other) {
  *ray_rb2vector2_ptr(self) = *ray_rb2vector2_ptr(other);
  return self;
}

/* @return [Float] The x position of the vector */
static
VALUE ray_vector2_x(VALUE self) {
  return rb_float_new(ray_rb2vector2_ptr(self)->x);
}

/* @return [Float] The y position of the vector */
static
VALUE ray_vector2_y(VALUE self) {
  return rb_float_new(ray_rb2vector2_ptr(self)->y);
}

/*
  @overload x=(x)
    Sets the x position of the vector

    @param [Float] x New x position
    @return [void]
*/
static
VALUE ray_vector2_set_x(VALUE self, VALUE x) {
  rb_check_frozen(self);
  ray_rb2vector2_ptr(self)->x = NUM2DBL(x);
  return x;
}

/*
  @overload y=(y)
    Sets the y position of the vector

    @param [Float] y New y position
    @return [void]
*/
static
VALUE ray_vector2_set_y(VALUE self, VALUE y) {
  rb_check_frozen(self);
  ray_rb2vector2_ptr(self)->y = NUM2DBL(y);
  return y;
}

static
VALUE ray_vector3_alloc(VALUE self) {
  say_vector3 *obj = malloc(sizeof(say_vector3));
  *obj = say_make_vector3(0, 0, 0);

  return Data_Wrap_Struct(self, NULL, free, obj);
}

/*
  @overload initialize(x = 0.0, y = 0.0, z = 0.0)
    @param [Float] x X position of the vector
    @param [Float] y Y postiion of the vector
    @param [Float] z Z postiion of the vector
*/
static
VALUE ray_vector3_init(int argc, VALUE *argv, VALUE self) {
  VALUE rb_x = Qnil, rb_y = Qnil, rb_z = Qnil;
  rb_scan_args(argc, argv, "03", &rb_x, &rb_y, &rb_z);

  say_vector3 *vector = ray_rb2vector3_ptr(self);
  if (!NIL_P(rb_x)) vector->x = NUM2DBL(rb_x);
  if (!NIL_P(rb_y)) vector->y = NUM2DBL(rb_y);
  if (!NIL_P(rb_z)) vector->z = NUM2DBL(rb_z);

  return self;
}

static
VALUE ray_vector3_init_copy(VALUE self, VALUE other) {
  *ray_rb2vector3_ptr(self) = *ray_rb2vector3_ptr(other);
  return self;
}

/* @return [Float] The x position of the vector */
static
VALUE ray_vector3_x(VALUE self) {
  return rb_float_new(ray_rb2vector3_ptr(self)->x);
}

/* @return [Float] The y position of the vector */
static
VALUE ray_vector3_y(VALUE self) {
  return rb_float_new(ray_rb2vector3_ptr(self)->y);
}

/* @return [Float] The z position of the vector */
static
VALUE ray_vector3_z(VALUE self) {
  return rb_float_new(ray_rb2vector3_ptr(self)->z);
}

/*
  @overload x=(x)
    Sets the x position of the vector

    @param [Float] x New x position
    @return [void]
*/
static
VALUE ray_vector3_set_x(VALUE self, VALUE x) {
  rb_check_frozen(self);
  ray_rb2vector3_ptr(self)->x = NUM2DBL(x);
  return x;
}

/*
  @overload y=(y)
    Sets the y position of the vector

    @param [Float] y New y position
    @return [void]
*/
static
VALUE ray_vector3_set_y(VALUE self, VALUE y) {
  rb_check_frozen(self);
  ray_rb2vector3_ptr(self)->y = NUM2DBL(y);
  return y;
}

/*
  @overload z=(z)
    Sets the z position of the vector

    @param [Float] z New z position
    @return [void]
*/
static
VALUE ray_vector3_set_z(VALUE self, VALUE z) {
  rb_check_frozen(self);
  ray_rb2vector3_ptr(self)->z = NUM2DBL(z);
  return z;
}

/*
  Document-class: Ray::Vector2

  This class can be used to represent either a 2D point (x, y) or a size WxH
  (hence the aliased methods).
*/

/*
  Document-class: Ray::Vector3

  Represents a (x, y, z) point in 3D space.
*/

void Init_ray_vector() {
  ray_cVector2 = rb_define_class_under(ray_mRay, "Vector2", rb_cObject);
  rb_define_alloc_func(ray_cVector2, ray_vector2_alloc);
  rb_define_method(ray_cVector2, "initialize", ray_vector2_init, -1);
  rb_define_method(ray_cVector2, "initialize_copy", ray_vector2_init_copy, 1);

  rb_define_method(ray_cVector2, "x", ray_vector2_x, 0);
  rb_define_method(ray_cVector2, "y", ray_vector2_y, 0);

  rb_define_method(ray_cVector2, "x=", ray_vector2_set_x, 1);
  rb_define_method(ray_cVector2, "y=", ray_vector2_set_y, 1);

  ray_cVector3 = rb_define_class_under(ray_mRay, "Vector3", rb_cObject);
  rb_define_alloc_func(ray_cVector3, ray_vector3_alloc);
  rb_define_method(ray_cVector3, "initialize", ray_vector3_init, -1);
  rb_define_method(ray_cVector3, "initialize_copy", ray_vector3_init_copy, 1);

  rb_define_method(ray_cVector3, "x", ray_vector3_x, 0);
  rb_define_method(ray_cVector3, "y", ray_vector3_y, 0);
  rb_define_method(ray_cVector3, "z", ray_vector3_z, 0);

  rb_define_method(ray_cVector3, "x=", ray_vector3_set_x, 1);
  rb_define_method(ray_cVector3, "y=", ray_vector3_set_y, 1);
  rb_define_method(ray_cVector3, "z=", ray_vector3_set_z, 1);
}
