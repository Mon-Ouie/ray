#include "ray.h"

VALUE ray_mGL = Qnil;
static VALUE ray_gl_primitives = Qnil;

/*
  @overload draw_arrays(primitive, first, count)
    @note Misusing this method can cause a crash.
    @param [Symbol] primitive Primitive to draw. Must be one of the folowing:
      points, line_strip, line_loop, lines, triangle_strip, triangle_fan,
      triangles.

    @param [Integer] first Identifier of the first vertex to draw.
    @param [Integer] count Amount of vertices to draw.
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
  @overload multi_draw_arrays(primitive, first, count)
    @param primitive (see #draw_arrays)
    @param [Ray::GL::IntArray] first Indices of the first vertex
    @param [Ray::GL::IntArray] count Ammount of vertices to draw
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

  rb_define_module_function(ray_mGL, "draw_arrays", ray_gl_draw_arrays, 3);
  rb_define_module_function(ray_mGL, "draw_elements", ray_gl_draw_elements, 3);

  rb_define_module_function(ray_mGL, "multi_draw_arrays",
                            ray_gl_multi_draw_arrays, 3);
  rb_define_module_function(ray_mGL, "multi_draw_elements",
                            ray_gl_multi_draw_elements, 3);
}
