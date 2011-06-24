#include "ray.h"

VALUE ray_cFont = Qnil;

say_font *ray_rb2font(VALUE obj) {
  if (!RAY_IS_A(obj, rb_path2class("Ray::Font"))) {
    rb_raise(rb_eTypeError, "can't convert %s into Ray::Font",
             RAY_OBJ_CLASSNAME(obj));
  }

  say_font *font = NULL;
  Data_Get_Struct(obj, say_font, font);

  return font;
}

static
VALUE ray_font_alloc(VALUE self) {
  say_font *obj = say_font_create();
  return Data_Wrap_Struct(self, NULL, say_font_free, obj);
}

/*
  @overload initialize(filename)
    @param [String] filename Name of the file to load the font from.
  @overload initialize(io)
    @param [#read] io IO object to read the font from.
*/
static
VALUE ray_font_init(VALUE self, VALUE arg) {
  say_font *font = ray_rb2font(self);

  if (rb_respond_to(arg, RAY_METH("read"))) {
    arg = rb_funcall(arg, RAY_METH("read"), 0);
    if (!say_font_load_from_memory(font, StringValuePtr(arg),
                                   RSTRING_LEN(arg))) {
      rb_raise(rb_eRuntimeError, "%s", say_error_get_last());
    }
  }
  else if (rb_respond_to(arg, RAY_METH("to_str"))) {
    if (!say_font_load_from_file(font, StringValuePtr(arg))) {
      rb_raise(rb_eRuntimeError, "%s", say_error_get_last());
    }
  }
  else {
    rb_raise(rb_eTypeError, "Can't convert %s into String",
             RAY_OBJ_CLASSNAME(arg));

  }

  return self;
}

/*
  @overload kerning(size, a, b)
    @param [Integer] size Size of the font.
    @param [Integer] a Codepoint of the first character.
    @param [Integer] b Codepoint of the second character.

    @return [Integer] Size there should be between the two characters. Most of
      the time, just 0, but can also be a negative number.
 */
static
VALUE ray_font_kerning(VALUE self, VALUE size, VALUE a, VALUE b) {
  say_font *font = ray_rb2font(self);
  size_t kern = say_font_get_kerning(font, NUM2ULONG(a), NUM2ULONG(b),
                                     NUM2ULONG(size));

  return INT2FIX(kern);
}

/*
  @overload line_height(size)
    @param [Integer] size Size of the font
    @return [Integer] Height of a line
*/
static
VALUE ray_font_line_height(VALUE self, VALUE size) {
  return INT2FIX(say_font_get_line_height(ray_rb2font(self), NUM2ULONG(size)));
}

void Init_ray_font() {
  ray_cFont = rb_define_class_under(ray_mRay, "Font", rb_cObject);
  rb_define_alloc_func(ray_cFont, ray_font_alloc);
  rb_define_method(ray_cFont, "initialize", ray_font_init, 1);

  rb_define_method(ray_cFont, "kerning", ray_font_kerning, 3);
  rb_define_method(ray_cFont, "line_height", ray_font_line_height, 1);
}
