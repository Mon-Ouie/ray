#include "ray.h"

VALUE ray_mGL = Qnil;
static VALUE ray_gl_primitives = Qnil;

/*
 * @overload draw_arrays(primitive, first, count)
 *   @param [Symbol] primitive Primitive to draw. Must be one of the folowing:
 *     points, line_strip, line_loop, lines, triangle_strip, triangle_fan,
 *     triangles.
 *
 *   @param [Integer] first Identifier of the first vertex to draw.
 *   @param [Integer] count Amount of vertices to draw.
 */
static
VALUE ray_gl_draw_arrays(VALUE self, VALUE primitive, VALUE first,
                         VALUE count) {

  glDrawArrays(NUM2INT(rb_hash_aref(ray_gl_primitives, primitive)),
               NUM2ULONG(first), NUM2ULONG(count));
  return Qnil;
}

/*
 * @overload draw_elements(primitive, count, index)
 *   @param primitive (see #draw_arrays)
 *   @param count     (see #draw_arrays)
 *   @param index     First index from the element array.
 */
static
VALUE ray_gl_draw_elements(VALUE self, VALUE primitive, VALUE count,
                           VALUE index) {
  glDrawElements(NUM2INT(rb_hash_aref(ray_gl_primitives, primitive)),
                 NUM2ULONG(count),
                 GL_UNSIGNED_INT, (void*)NUM2ULONG(index));
  return Qnil;
}

/*
 * @overload draw_arrays_instanced(primitive, first, count, instance_count)
 *   @param [Symbol] primitive (see #draw_arrays)
 *   @param [Integer] first (see #draw_arrays)
 *   @param [Integer] count (see #draw_arrays)
 *   @param [Integer] instance_count Amount of instances to draw
 */
static
VALUE ray_gl_draw_arrays_instanced(VALUE self, VALUE primitive, VALUE first,
                                   VALUE count, VALUE instance_count) {
  say_context_ensure();
  if (glDrawArraysInstanced) {
    glDrawArraysInstanced(NUM2INT(rb_hash_aref(ray_gl_primitives, primitive)),
                          NUM2ULONG(first), NUM2ULONG(count),
                          NUM2ULONG(instance_count));
  }
  else
    rb_raise(rb_eRuntimeError, "GL_ARB_draw_instanced is not supported");

  return Qnil;
}

/*
 * @overload draw_elements_instanced(primitive, count, index, instance_count)
 *   @param primitive (see #draw_arrays)
 *   @param count     (see #draw_arrays)
 *   @param index     (see #draw_elements)
 *   @param instance_count (see #draw_arrays_instanced)
 */
static
VALUE ray_gl_draw_elements_instanced(VALUE self, VALUE primitive, VALUE count,
                                     VALUE index, VALUE instance_count) {
  say_context_ensure();
  if (glDrawElementsInstanced) {
    glDrawElementsInstanced(NUM2INT(rb_hash_aref(ray_gl_primitives, primitive)),
                            NUM2ULONG(count),
                            GL_UNSIGNED_INT, (void*)NUM2ULONG(index),
                            NUM2ULONG(instance_count));
  }
  else
    rb_raise(rb_eRuntimeError, "GL_ARB_draw_instanced is not supported");

  return Qnil;
}


/*
 * @overload multi_draw_arrays(primitive, first, count)
 *   @param primitive (see #draw_arrays)
 *   @param [Ray::GL::IntArray] first Indices of the first vertex
 *   @param [Ray::GL::IntArray] count Ammount of vertices to draw
 */
static
VALUE ray_gl_multi_draw_arrays(VALUE self, VALUE primitive, VALUE rb_first,
                               VALUE rb_count) {
  say_array *first = ray_rb2int_array(rb_first);
  say_array *count = ray_rb2int_array(rb_count);

  size_t size = say_array_get_size(first);

  if (size != say_array_get_size(count))
    rb_raise(rb_eArgError, "first and count arrays should have the same size");

  glMultiDrawArrays(NUM2INT(rb_hash_aref(ray_gl_primitives, primitive)),
                    say_array_get(first, 0), say_array_get(count, 0),
                    size);

  return Qnil;
}

/*
 * @overload multi_draw_elements(primitive, count, index)
 *   @param primitive (see #draw_arrays)
 *   @param count     (see #multi_draw_arrays)
 *   @param [Ray::GL::IntArray] index First indices from the element array for
 *     each draw
 */
static
VALUE ray_gl_multi_draw_elements(VALUE self, VALUE primitive, VALUE rb_count,
                                 VALUE rb_index) {
  say_array *index = ray_rb2int_array(rb_index);
  say_array *count = ray_rb2int_array(rb_count);

  size_t size = say_array_get_size(index);

  if (size != say_array_get_size(count))
    rb_raise(rb_eArgError, "index and count arrays should have the same size");

  glMultiDrawElements(NUM2INT(rb_hash_aref(ray_gl_primitives, primitive)),
                      say_array_get(count, 0),
                      GL_UNSIGNED_INT, (const GLvoid**)say_array_get(count, 0),
                      size);
  return Qnil;
}

/* @return [Integer] Size of the depth buffer, in bits */
static
VALUE ray_gl_depth_size(VALUE self) {
  return ULONG2NUM(say_context_get_config()->depth_size);
}

/* @return [Integer] Size of the stencil buffer, in bits */
static
VALUE ray_gl_stencil_size(VALUE self) {
  return ULONG2NUM(say_context_get_config()->stencil_size);
}

/* @return [Integer] Major version number. Ignored if less than 3. */
static
VALUE ray_gl_major_version(VALUE self) {
  return ULONG2NUM(say_context_get_config()->major_version);
}

/* @return [Integer] Minor version number */
static
VALUE ray_gl_minor_version(VALUE self) {
  return ULONG2NUM(say_context_get_config()->minor_version);
}

/* @return [Bollean] True when a core OpenGL profile must be used */
static
VALUE ray_gl_core_profile(VALUE self) {
  return say_context_get_config()->core_profile ? Qtrue : Qfalse;
}

/* @see depth_size */
static
VALUE ray_gl_set_depth_size(VALUE self, VALUE val) {
  say_context_get_config()->depth_size = NUM2ULONG(val);
  return val;
}

/* @see stencil_size */
static
VALUE ray_gl_set_stencil_size(VALUE self, VALUE val) {
  say_context_get_config()->stencil_size = NUM2ULONG(val);
  return val;
}

/* @see major_version */
static
VALUE ray_gl_set_major_version(VALUE self, VALUE val) {
  say_context_get_config()->major_version = NUM2ULONG(val);
  return val;
}

/* @see minor_version */
static
VALUE ray_gl_set_minor_version(VALUE self, VALUE val) {
  say_context_get_config()->minor_version = NUM2ULONG(val);
  return val;
}

/* @see core_profile? */
static
VALUE ray_gl_set_core_profile(VALUE self, VALUE val) {
  say_context_get_config()->core_profile = RTEST(val);
  return val;
}

void Init_ray_gl() {
  ray_mGL = rb_define_module_under(ray_mRay, "GL");

  ray_gl_primitives = rb_hash_new();
  rb_hash_aset(ray_gl_primitives, RAY_SYM("points"), INT2FIX(GL_POINTS));
  rb_hash_aset(ray_gl_primitives, RAY_SYM("line_strip"),
               INT2FIX(GL_LINE_STRIP));
  rb_hash_aset(ray_gl_primitives, RAY_SYM("line_loop"), INT2FIX(GL_LINE_LOOP));
  rb_hash_aset(ray_gl_primitives, RAY_SYM("lines"), INT2FIX(GL_LINES));
  rb_hash_aset(ray_gl_primitives, RAY_SYM("triangle_strip"),
               INT2FIX(GL_TRIANGLE_STRIP));
  rb_hash_aset(ray_gl_primitives, RAY_SYM("triangle_fan"),
               INT2FIX(GL_TRIANGLE_FAN));
  rb_hash_aset(ray_gl_primitives, RAY_SYM("triangles"), INT2FIX(GL_TRIANGLES));

  /* @return [Hash] Available primitives. */
  rb_define_const(ray_mGL, "Primitives", ray_gl_primitives);

  rb_define_module_function(ray_mGL, "depth_size", ray_gl_depth_size, 0);
  rb_define_module_function(ray_mGL, "depth_size=", ray_gl_set_depth_size, 1);

  rb_define_module_function(ray_mGL, "stencil_size", ray_gl_stencil_size, 0);
  rb_define_module_function(ray_mGL, "stencil_size=", ray_gl_set_stencil_size,
                            1);

  rb_define_module_function(ray_mGL, "major_version", ray_gl_major_version, 0);
  rb_define_module_function(ray_mGL, "major_version=", ray_gl_set_major_version,
                            1);

  rb_define_module_function(ray_mGL, "minor_version", ray_gl_minor_version, 0);
  rb_define_module_function(ray_mGL, "minor_version=", ray_gl_set_minor_version,
                            1);

  rb_define_module_function(ray_mGL, "core_profile?", ray_gl_core_profile, 0);
  rb_define_module_function(ray_mGL, "core_profile=", ray_gl_set_core_profile, 1);

  rb_define_module_function(ray_mGL, "draw_arrays", ray_gl_draw_arrays, 3);
  rb_define_module_function(ray_mGL, "draw_elements", ray_gl_draw_elements, 3);

  rb_define_module_function(ray_mGL, "draw_arrays_instanced",
                            ray_gl_draw_arrays_instanced, 4);
  rb_define_module_function(ray_mGL, "draw_elements_instanced",
                            ray_gl_draw_elements_instanced, 4);

  rb_define_module_function(ray_mGL, "multi_draw_arrays",
                            ray_gl_multi_draw_arrays, 3);
  rb_define_module_function(ray_mGL, "multi_draw_elements",
                            ray_gl_multi_draw_elements, 3);
}
