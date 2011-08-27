#include "ray.h"

VALUE ray_cMatrix = Qnil;

VALUE ray_matrix2rb(say_matrix *matrix) {
  say_matrix *copy = say_matrix_identity();
  say_matrix_set_content(copy, matrix->content);
  return Data_Wrap_Struct(rb_path2class("Ray::Matrix"), NULL, say_matrix_free,
                          copy);
}

say_matrix *ray_rb2matrix(VALUE matrix) {
  if (!RAY_IS_A(matrix, rb_path2class("Ray::Matrix"))) {
    rb_raise(rb_eTypeError, "can't convert %s into Ray::Matrix",
             RAY_OBJ_CLASSNAME(matrix));
  }

  say_matrix *obj = NULL;
  Data_Get_Struct(matrix, say_matrix, obj);

  return obj;
}

static
void ray_matrix_assert_valid_pos(int x, int y) {
  if (x < 0 || x >= 4)
    rb_raise(rb_eRangeError, "%d is not in the 0...4 range", x);
  else if (y < 0 || y >= 4)
    rb_raise(rb_eRangeError, "%d is not in the 0...4 range", y);
}

static
VALUE ray_matrix_alloc(VALUE self) {
  say_matrix *obj = say_matrix_identity();
  return Data_Wrap_Struct(self, NULL, say_matrix_free, obj);
}

/*
  @overload [](x, y)
    @param [Integer] x X position of the cell
    @param [Integer] y Y position of the cell

    @return [Float] Content of the cell at the given position

    @raise [ArgumentError] If at least one of x and y is not included in the
      0...4 range
*/
static
VALUE ray_matrix_get(VALUE self, VALUE rb_x, VALUE rb_y) {
  int x = NUM2INT(rb_x), y = NUM2INT(rb_y);
  ray_matrix_assert_valid_pos(x, y);

  say_matrix *matrix = ray_rb2matrix(self);
  return rb_float_new(say_matrix_get(matrix, x, y));
}

/*
  @overload []=(x, y, val)

  Sets a new value to one of the cell of the matrix

  @param [Integer] x X position of the cell
  @param [Integer] y Y position of the cell

  @param [Float] val New content for the cell

  @return [void]

  @raise [ArgumentError] If at least one of x and y is not included in the
    0...4 range
*/
static
VALUE ray_matrix_set(VALUE self, VALUE rb_x, VALUE rb_y, VALUE val) {
  rb_check_frozen(self);

  int x = NUM2INT(rb_x), y = NUM2INT(rb_y);
  ray_matrix_assert_valid_pos(x, y);

  say_matrix *matrix = ray_rb2matrix(self);
  say_matrix_set(matrix, x, y, NUM2DBL(val));

  return val;
}

/*
  @return [Array<Float>] The 16 cells of the matrix.

  @example
    p Matrix.new.content
    # result:
    #   [1, 0, 0, 0,
    #    0, 1, 0, 0,
    #    0, 0, 1, 0,
    #    0, 0, 0, 1]
*/
static
VALUE ray_matrix_content(VALUE self) {
  float *content = say_matrix_get_content(ray_rb2matrix(self));

  VALUE result = rb_ary_new();
  for (int i = 0; i < 16; i++)
    rb_ary_push(result, rb_float_new(content[i]));

  return result;
}

/*
  @overload content=(val)

  Changes the whole content of the matrix.
  @param [Array<Float>] val The 16 cells of the matrix

  @return [void]
*/
static
VALUE ray_matrix_set_content(VALUE self, VALUE rb_content) {
  rb_check_frozen(self);

  if (RARRAY_LEN(rb_content) != 16) {
    rb_raise(rb_eArgError, "size of array is %ld, 16 expected",
             RARRAY_LEN(rb_content));
  }

  say_matrix *matrix = ray_rb2matrix(self);

  float content[16];
  for (int i = 0; i < 16; i++)
    content[i] = NUM2DBL(RAY_ARRAY_AT(rb_content, i));

  say_matrix_set_content(matrix, content);

  return rb_content;
}

/*
  Resets a matrix to identity.
  @return [self]
*/
static
VALUE ray_matrix_reset(VALUE self) {
  rb_check_frozen(self);
  say_matrix_reset(ray_rb2matrix(self));
  return self;
}

/*
  @overload multiply_by!(other)
    Multiplies this matrix by another one and stores the result in self.

    @param [Matrix] other A matrix to multiply self by.
    @return [self]

    @see Matrix#*
*/
static
VALUE ray_matrix_multiply_by(VALUE self, VALUE other) {
  rb_check_frozen(self);
  say_matrix_multiply_by(ray_rb2matrix(self), ray_rb2matrix(other));
  return self;
}

/*
  @overload transform(vector)
    Applies this transformation matrix to a vector.

    @param [#to_vector3] Vector to apply the transformations to
    @return [Ray::Vector3] A new vector to which the transformations have been
      applied.
*/
static
VALUE ray_matrix_transform(VALUE self, VALUE vector) {
  say_vector3 ret = say_matrix_transform(ray_rb2matrix(self),
                                         ray_convert_to_vector3(vector));

  return ray_vector3_to_rb(ret);
}

/*
  @param [Integer] x X position of the element
  @param [Integer] y Y position of the element

  @return [Float] The cofactor for that element
*/
static
VALUE ray_matrix_cofactor(VALUE self, VALUE x, VALUE y) {
  return rb_float_new(say_matrix_cofactor(ray_rb2matrix(self),
                                          NUM2INT(x),
                                          NUM2INT(y)));
}

/*
  @return [Ray::Matrix] the comatrix of the current one.
*/
static
VALUE ray_matrix_comatrix(VALUE self) {
  say_matrix *comatrix = say_matrix_comatrix(ray_rb2matrix(self));
  return Data_Wrap_Struct(rb_class_of(self), NULL, say_matrix_free, comatrix);
}


/*
  @return [Ray::Matrix] The inverse matrix of the current one.
*/
static
VALUE ray_matrix_inverse(VALUE self) {
  say_matrix *inverse = say_matrix_inverse(ray_rb2matrix(self));
  return Data_Wrap_Struct(rb_class_of(self), NULL, say_matrix_free, inverse);
}

/*
  @overload translate(vector)
    Applies a translation to the matrix.

    @param [Ray::Vector3] vector Translation to apply
    @return [void]
*/
static
VALUE ray_matrix_translate(VALUE self, VALUE rb_vector) {
  rb_check_frozen(self);

  say_vector3 vector = ray_convert_to_vector3(rb_vector);
  say_matrix_translate_by(ray_rb2matrix(self), vector.x, vector.y, vector.z);

  return self;
}

/*
  @overload scale(vector)
    Scales the matrix. Sizes are multiplied by the vector.

    @param [Ray::Vector3] vector Vector multiplying the sizes
    @return [void]
*/
static
VALUE ray_matrix_scale(VALUE self, VALUE rb_vector) {
  rb_check_frozen(self);

  say_vector3 vector = ray_convert_to_vector3(rb_vector);
  say_matrix_scale_by(ray_rb2matrix(self), vector.x, vector.y, vector.z);

  return self;
}

/*
  @overload rotate(angle, vector)
    Rotates the matrix.

    @param [Float] angle Angle of the rotation in degrees
    @param [Ray::Vector3] vector Vector followed by the rotation

    @return [void]
*/
static
VALUE ray_matrix_rotate(VALUE self, VALUE angle, VALUE rb_vector) {
  rb_check_frozen(self);

  say_vector3 vector = ray_convert_to_vector3(rb_vector);
  say_matrix_rotate(ray_rb2matrix(self), NUM2DBL(angle),
                    vector.x, vector.y, vector.z);

  return self;
}

/*
  @overload look_at(eye, center, up)
    Creates a view matrix, usually for 3D.

    @param [Ray::Vector3] eye Position of the eye
    @param [Ray::Vector3] center Reference point
    @param [Ray::Vector3] up
 */
static
VALUE ray_matrix_look_at(VALUE self, VALUE eye, VALUE center, VALUE up) {
  say_matrix *mat = ray_rb2matrix(self);

  say_vector3 c_eye    = ray_convert_to_vector3(eye);
  say_vector3 c_center = ray_convert_to_vector3(center);
  say_vector3 c_up     = ray_convert_to_vector3(up);

  say_matrix_look_at(mat,
                     c_eye.x, c_eye.y, c_eye.z,
                     c_center.x, c_center.y, c_center.z,
                     c_up.x, c_up.y, c_up.z);

  return self;
}

/*
  @overload orthogonal(left, right, bottom, top, near, far)
    Setup an orthogonal projection.

    @param [Float] left X position of the left corners
    @param [Float] right X position of the right corners

    @param [Float] bottom Y position of the bottom corners
    @param [Float] top Y position of the top corners

    @param [Float] near Nearest z value that can be seen
    @param [Float] far Farthest z valaue that can be seen
*/
static
VALUE ray_matrix_orthogonal(VALUE self,
                            VALUE left, VALUE right,
                            VALUE bottom, VALUE top,
                            VALUE z_near, VALUE z_far) {
  rb_check_frozen(self);

  say_matrix_set_ortho(ray_rb2matrix(self),
                       NUM2DBL(left), NUM2DBL(right),
                       NUM2DBL(bottom), NUM2DBL(top),
                       NUM2DBL(z_near), NUM2DBL(z_far));

  return self;
}

/*

  @overload perspective(fovy, aspect, near, far)
    Setup a perspective projection.

    @param [Float] fovy Field of view in degrees, in the y direction.
    @param [Float] aspect Width divided by height
    @param [Float] near Distance from the viewer to the nearest point
    @param [Float] far Distance from the viewer to the farthest point
*/
static
VALUE ray_matrix_perspective(VALUE self,
                             VALUE fovy, VALUE aspect,
                             VALUE z_near, VALUE z_far) {
  rb_check_frozen(self);

  say_matrix_set_perspective(ray_rb2matrix(self),
                             NUM2DBL(fovy), NUM2DBL(aspect),
                             NUM2DBL(z_near), NUM2DBL(z_far));

  return self;
}

/*
 * @overload set_transformation(origin, pos, z, scale, angle)
 *   Resets the matrix content to a 2D transformation matrix
 *
 *   @param [Vector2] origin Origin of all the transformations
 *   @param [Vector2] pos Position of the object
 *   @param [Float] z Z ordering
 *   @param [Vector2] scale Scaling factor
 *   @param [Float] angle Rotation
 */
static
VALUE ray_matrix_set_transformation(VALUE self,
                                    VALUE origin,
                                    VALUE pos, VALUE z,
                                    VALUE scale,
                                    VALUE angle) {
  rb_check_frozen(self);

  say_matrix_set_transformation(ray_rb2matrix(self),
                                ray_convert_to_vector2(origin),
                                ray_convert_to_vector2(pos),
                                NUM2DBL(z),
                                ray_convert_to_vector2(scale),
                                NUM2DBL(angle));

  return self;
}

/*
  Document-class: Ray::Matrix

  Ray::Matrix represents a 4x4 matrices. Such matrices can be used to aplly
  transformations to a point. It contains methods creating such transformation
  matrices directly, or adding new transformations to the current matrix.
*/

void Init_ray_matrix() {
  ray_cMatrix = rb_define_class_under(ray_mRay, "Matrix", rb_cObject);

  rb_define_alloc_func(ray_cMatrix, ray_matrix_alloc);

  rb_define_method(ray_cMatrix, "[]", ray_matrix_get, 2);
  rb_define_method(ray_cMatrix, "[]=", ray_matrix_set, 3);

  rb_define_method(ray_cMatrix, "content", ray_matrix_content, 0);
  rb_define_method(ray_cMatrix, "content=", ray_matrix_set_content, 1);

  rb_define_method(ray_cMatrix, "reset", ray_matrix_reset, 0);

  rb_define_method(ray_cMatrix, "multiply_by!", ray_matrix_multiply_by, 1);
  rb_define_method(ray_cMatrix, "transform", ray_matrix_transform, 1);

  rb_define_method(ray_cMatrix, "cofactor", ray_matrix_cofactor, 2);
  rb_define_method(ray_cMatrix, "comatrix", ray_matrix_comatrix, 0);
  rb_define_method(ray_cMatrix, "inverse", ray_matrix_inverse, 0);

  rb_define_method(ray_cMatrix, "translate", ray_matrix_translate, 1);
  rb_define_method(ray_cMatrix, "scale", ray_matrix_scale, 1);
  rb_define_method(ray_cMatrix, "rotate", ray_matrix_rotate, 2);
  rb_define_method(ray_cMatrix, "look_at", ray_matrix_look_at, 3);
  rb_define_method(ray_cMatrix, "orthogonal", ray_matrix_orthogonal, 6);
  rb_define_method(ray_cMatrix, "perspective", ray_matrix_perspective, 4);
  rb_define_method(ray_cMatrix, "set_transformation",
                   ray_matrix_set_transformation, 5);
}
