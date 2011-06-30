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
}
