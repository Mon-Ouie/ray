#include "ray.h"

VALUE ray_cDrawable = Qnil;

static
void ray_drawable_actual_matrix_proc(void *data, say_matrix *matrix) {
  say_drawable *drawable = ((ray_drawable*)data)->drawable;

  VALUE rb_obj = (VALUE)say_drawable_get_other_data(drawable);
  VALUE proc   = rb_iv_get(rb_obj, "@matrix_proc");

  VALUE rb_matrix = rb_funcall(proc, RAY_METH("call"), 1, rb_obj);

  memcpy(matrix->content, ray_rb2matrix(rb_matrix)->content,
         sizeof(say_matrix));
}

static
void ray_drawable_fill_proc(void *drawable_ptr, void *vertex_ptr) {
  ray_drawable *drawable = drawable_ptr;
  uint8_t      *vertices = vertex_ptr;

  VALUE ary   = rb_funcall(drawable->obj, RAY_METH("fill_vertices"), 0);
  VALUE klass = rb_iv_get(drawable->obj, "@vertex_type_class");

  size_t size = say_drawable_get_vertex_count(drawable->drawable);
  for (size_t i = 0; i < size; i++) {
    VALUE vertex = RAY_ARRAY_AT(ary, i);

    if (!RAY_IS_A(vertex, klass)) {
      rb_raise(rb_eTypeError, "Can't convert %s into %s",
               RAY_OBJ_CLASSNAME(vertex), rb_class2name(klass));
    }

    uint8_t *data = NULL;
    Data_Get_Struct(vertex, uint8_t, data);

    memcpy(vertices, data, drawable->vsize);
    vertices += drawable->vsize;
  }
}

void ray_drawable_shader_proc(void *data, say_shader *shader) {
  /* Works because the offset will be the same anyway */
  say_drawable *drawable = ((ray_drawable*)data)->drawable;

  VALUE rb_obj = (VALUE)say_drawable_get_other_data(drawable);
  VALUE hash   = rb_iv_get(rb_obj, "@shader_attributes");

  if (!NIL_P(hash)) {
    /* Create a ruby shader object to call merge */
    VALUE rb_shader = ray_shader2rb(shader, Qnil);
    rb_funcall(rb_shader, RAY_METH("merge"), 1, hash);
  }
}

static
void ray_drawable_render_proc(void *data, size_t first, size_t index) {
  ray_drawable *drawable = (ray_drawable*)data;
  rb_funcall(drawable->obj, RAY_METH("render"), 2,
             ULONG2NUM(first), ULONG2NUM(index));
}

ray_drawable *ray_rb2full_drawable(VALUE obj) {
  if (rb_obj_is_kind_of(obj, rb_path2class("Ray::Text"))    ||
      rb_obj_is_kind_of(obj, rb_path2class("Ray::Sprite"))  ||
      rb_obj_is_kind_of(obj, rb_path2class("Ray::Polygon")) ||
      !rb_obj_is_kind_of(obj, rb_path2class("Ray::Drawable"))) {
    rb_raise(rb_eTypeError, "can't get drawable pointer from %s",
             RAY_OBJ_CLASSNAME(obj));
  }

  ray_drawable *ptr = NULL;
  Data_Get_Struct(obj, ray_drawable, ptr);

  if (!ptr->drawable) {
    rb_raise(rb_eRuntimeError, "trying to use an uninitialized drawable");
  }

  return ptr;
}

static
void ray_drawable_indices_fill_proc(void *data, GLuint *ptr, size_t first) {
  ray_drawable *drawable = (ray_drawable*)data;
  VALUE array = rb_funcall(drawable->obj, RAY_METH("fill_indices"), 1,
                           ULONG2NUM(first));

  mo_array *c_array      = ray_rb2int_array(array);
  size_t     size        = say_drawable_get_index_count(drawable->drawable);
  size_t     actual_size = c_array->size;

  if (actual_size < size) {
    rb_raise(rb_eRuntimeError, "received %zu indices, expected %zu",
             actual_size, size);
  }

  memcpy(ptr, mo_array_at(c_array, 0), sizeof(GLuint) * size);
}

say_drawable *ray_rb2drawable(VALUE obj) {
  if (RAY_IS_A(obj, rb_path2class("Ray::Polygon")))
    return ray_rb2polygon(obj)->drawable;
  else if (RAY_IS_A(obj, rb_path2class("Ray::Sprite")))
    return ray_rb2sprite(obj)->drawable;
  else if (RAY_IS_A(obj, rb_path2class("Ray::Text")))
    return ray_rb2text(obj)->drawable;
  else {
    return ray_rb2full_drawable(obj)->drawable;
  }
}

static
void ray_drawable_free(ray_drawable *drawable) {
  if (drawable->drawable)
    say_drawable_free(drawable->drawable);
  free(drawable);
}

static
VALUE ray_drawable_alloc(VALUE self) {
  ray_drawable *obj = malloc(sizeof(ray_drawable));

  VALUE rb = Data_Wrap_Struct(self, NULL, ray_drawable_free, obj);

  obj->drawable = NULL;
  obj->obj      = rb;

  return rb;
}

/*
  @overload initialize(vertex_class = Ray::Vertex)
    @param [Class] vertex_class Class of the vertices.
*/
static
VALUE ray_drawable_init(int argc, VALUE *argv, VALUE self) {
  if (rb_obj_is_kind_of(self, rb_path2class("Ray::Text"))   ||
      rb_obj_is_kind_of(self, rb_path2class("Ray::Sprite")) ||
      rb_obj_is_kind_of(self, rb_path2class("Ray::Polygon"))) {
    rb_raise(rb_eTypeError, "can't get drawable pointer from %s",
             RAY_OBJ_CLASSNAME(self));
  }

  ray_drawable *obj = NULL;
  Data_Get_Struct(self, ray_drawable, obj);

  VALUE arg = Qnil;
  rb_scan_args(argc, argv, "01", &arg);

  size_t id = NIL_P(arg) ? 0 : ray_get_vtype(arg);

  obj->drawable = say_drawable_create(id);
  say_drawable_set_custom_data(obj->drawable, obj);
  say_drawable_set_other_data(obj->drawable, (void*)self);
  say_drawable_set_fill_proc(obj->drawable, ray_drawable_fill_proc);
  say_drawable_set_shader_proc(obj->drawable, ray_drawable_shader_proc);
  say_drawable_set_render_proc(obj->drawable, ray_drawable_render_proc);
  say_drawable_set_index_fill_proc(obj->drawable,
                                   ray_drawable_indices_fill_proc);
  say_drawable_set_changed(obj->drawable);

  rb_iv_set(self, "@vertex_type_class", NIL_P(arg) ?
            rb_path2class("Ray::Vertex") : arg);

  obj->vsize = say_vertex_type_get_size(say_get_vertex_type(id));

  return self;
}

void ray_drawable_copy_attr(VALUE self, VALUE orig) {
  VALUE attr = rb_iv_get(orig, "@shader_attributes");
  if (!NIL_P(attr))
    rb_iv_set(self, "@shader_attributes", rb_obj_dup(attr));
}

static
VALUE ray_drawable_init_copy(VALUE self, VALUE orig) {
  if (rb_obj_is_kind_of(self, rb_path2class("Ray::Text"))   ||
      rb_obj_is_kind_of(self, rb_path2class("Ray::Sprite")) ||
      rb_obj_is_kind_of(self, rb_path2class("Ray::Polygon"))) {
    rb_raise(rb_eTypeError, "can't get drawable pointer from %s",
             RAY_OBJ_CLASSNAME(self));
  }

  ray_drawable *obj = NULL;
  Data_Get_Struct(self, ray_drawable, obj);

  ray_drawable *other = ray_rb2full_drawable(orig);

  size_t vid = say_drawable_get_vertex_type(other->drawable);
  obj->drawable = say_drawable_create(vid);
  say_drawable_set_custom_data(obj->drawable, obj);
  say_drawable_set_other_data(obj->drawable, (void*)self);
  say_drawable_copy(obj->drawable, other->drawable);
  say_drawable_set_changed(obj->drawable);

  rb_iv_set(self, "@vertex_type_class", rb_iv_get(orig, "@vertex_type_class"));
  obj->vsize = other->vsize;

  ray_drawable_copy_attr(self, orig);

  return self;
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
  say_drawable_set_angle(ray_rb2drawable(self), NUM2DBL(val));
  return val;
}

/*
 * @return [Proc, nil] Proc used to generate matrices, if any.
 */
static
VALUE ray_drawable_matrix_proc(VALUE self) {
  return rb_iv_get(self, "@matrix_proc");
}

/*
 * @overload matrix_proc=(val)
 *   Sets the proc used to generate matrices. When set to nil, disables
 *   custom proc. The proc is called with the drawable to generate a matrix
 *   for, and should return that matrix.
 *
 *   @param [Proc, nil] val
 */
static
VALUE ray_drawable_set_matrix_proc(VALUE self, VALUE val) {
  say_drawable *drawable = ray_rb2drawable(self);

  if (NIL_P(val))
    say_drawable_set_matrix_proc(drawable, NULL);
  else
    say_drawable_set_matrix_proc(drawable, ray_drawable_actual_matrix_proc);

  rb_iv_set(self, "@matrix_proc", val);
  return val;
}

/*
 * Marks the drawable's matrix as changed. This is meant to be called when you
 * want your matrix proc to be called the next time the matrix is accessed.
 */
static
VALUE ray_drawable_matrix_changed(VALUE self) {
  say_drawable_set_matrix_changed(ray_rb2drawable(self));
  return self;
}

/*
  @return [Ray::Matrix] The transformation matrix used by this object.
*/
static
VALUE ray_drawable_matrix(VALUE self) {
  return ray_matrix2rb(say_drawable_get_matrix(ray_rb2drawable(self)));
}

/*
  @overload matrix=(val)
    Sets the current matrix to a custom one, making Ray ignore attributes
    setting that change the transformation matrix.

    Setting this to nil will cause Ray to start using the actual transformation
    matrix.

    @param [Ray::Matrix, nil] val
*/
static
VALUE ray_drawable_set_matrix(VALUE self, VALUE val) {
  say_drawable *drawable = ray_rb2drawable(self);

  if (NIL_P(val))
    say_drawable_set_matrix(drawable, NULL);
  else
    say_drawable_set_matrix(drawable, ray_rb2matrix(val));

  return val;
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

static
VALUE ray_drawable_set_vertex_count(VALUE self, VALUE val) {
  ray_drawable *drawable = ray_rb2full_drawable(self);
  say_drawable_set_vertex_count(drawable->drawable, NUM2ULONG(val));
  return val;
}

/* @return [Integer] Amount of vertices used by this drawable */
static
VALUE ray_drawable_vertex_count(VALUE self) {
  say_drawable *drawable = ray_rb2drawable(self);
  size_t count = say_drawable_get_vertex_count(drawable);
  return ULONG2NUM(count);
}

static
VALUE ray_drawable_set_index_count(VALUE self, VALUE val) {
  ray_drawable *drawable = ray_rb2full_drawable(self);
  say_drawable_set_index_count(drawable->drawable, NUM2ULONG(val));
  return val;
}

/* @return [Integer] Amount of indices used by this drawable */
static
VALUE ray_drawable_index_count(VALUE self) {
  say_drawable *drawable = ray_rb2drawable(self);
  size_t count = say_drawable_get_index_count(drawable);
  return ULONG2NUM(count);
}


/* @return [true, flase] true if the drawable has changed, and vertices must be
 *   updated. */
static
VALUE ray_drawable_has_changed(VALUE self) {
  return say_drawable_has_changed(ray_rb2drawable(self)) ? Qtrue : Qfalse;
}

/* Marks the object as changed, meaning vertices must be updated. */
static
VALUE ray_drawable_set_changed(VALUE self) {
  say_drawable_set_changed(ray_rb2drawable(self));
  return self;
}

/* @return [true, flase] true if the drawable is textured */
static
VALUE ray_drawable_is_textured(VALUE self) {
  return say_drawable_is_textured(ray_rb2drawable(self)) ? Qtrue : Qfalse;
}

/* Marks the object as changed, meaning vertices must be updated. */
static
VALUE ray_drawable_set_textured(VALUE self, VALUE val) {
  say_drawable_set_textured(ray_rb2drawable(self), RTEST(val));
  return self;
}

/*
 * @overload blend_mode=(mode)
 *   Sets blend mode
 *
 *   Ray supports 3 differents blend mode: :add, :multiply, and :alpha (also the
 *   default). Blending can also be disabled using :none.
 *
 *   @param [Symbol] mode New blend mode
 */
static
VALUE ray_drawable_set_blend_mode(VALUE self, VALUE mode) {
  say_blend_mode c_mode = SAY_BLEND_NO;
  if (mode == RAY_SYM("alpha"))
    c_mode = SAY_BLEND_ALPHA;
  else if (mode == RAY_SYM("add"))
    c_mode = SAY_BLEND_ADD;
  else if (mode == RAY_SYM("multiply"))
    c_mode = SAY_BLEND_MULTIPLY;

  say_drawable_set_blend_mode(ray_rb2drawable(self), c_mode);
  return mode;
}

/* @see blend_mode= */
static
VALUE ray_drawable_blend_mode(VALUE self) {
  say_blend_mode mode = say_drawable_get_blend_mode(ray_rb2drawable(self));

  switch (mode) {
  case SAY_BLEND_NO: return RAY_SYM("none");
  case SAY_BLEND_ALPHA: return RAY_SYM("alpha");
  case SAY_BLEND_ADD: return RAY_SYM("add");
  case SAY_BLEND_MULTIPLY: return RAY_SYM("multiply");
  }

  return Qnil; /* should never happen */
}

void Init_ray_drawable() {
  ray_cDrawable = rb_define_class_under(ray_mRay, "Drawable", rb_cObject);
  rb_define_alloc_func(ray_cDrawable, ray_drawable_alloc);
  rb_define_method(ray_cDrawable, "initialize", ray_drawable_init, -1);
  rb_define_method(ray_cDrawable, "initialize_copy", ray_drawable_init_copy, 1);

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

  rb_define_method(ray_cDrawable, "matrix_proc", ray_drawable_matrix_proc, 0);
  rb_define_method(ray_cDrawable, "matrix_proc=", ray_drawable_set_matrix_proc,
                   1);

  rb_define_method(ray_cDrawable, "matrix_changed!", ray_drawable_matrix_changed, 0);

  rb_define_method(ray_cDrawable, "matrix", ray_drawable_matrix, 0);
  rb_define_method(ray_cDrawable, "matrix=", ray_drawable_set_matrix, 1);
  rb_define_method(ray_cDrawable, "transform", ray_drawable_transform, 1);

  rb_define_method(ray_cDrawable, "shader", ray_drawable_shader, 0);
  rb_define_method(ray_cDrawable, "shader=", ray_drawable_set_shader, 1);

  rb_define_method(ray_cDrawable, "vertex_count=",
                   ray_drawable_set_vertex_count, 1);
  rb_define_method(ray_cDrawable, "vertex_count", ray_drawable_vertex_count, 0);

  rb_define_method(ray_cDrawable, "index_count=", ray_drawable_set_index_count,
                   1);
  rb_define_method(ray_cDrawable, "index_count", ray_drawable_index_count, 0);

  rb_define_method(ray_cDrawable, "changed!", ray_drawable_set_changed, 0);
  rb_define_method(ray_cDrawable, "changed?", ray_drawable_has_changed, 0);

  rb_define_method(ray_cDrawable, "textured=", ray_drawable_set_textured, 1);
  rb_define_method(ray_cDrawable, "textured?", ray_drawable_is_textured, 0);

  rb_define_method(ray_cDrawable, "blend_mode=", ray_drawable_set_blend_mode,
                   1);
  rb_define_method(ray_cDrawable, "blend_mode", ray_drawable_blend_mode, 0);
}
