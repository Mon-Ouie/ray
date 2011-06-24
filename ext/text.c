#include "ray.h"

VALUE ray_cText = Qnil;

say_text *ray_rb2text(VALUE obj) {
  if (!RAY_IS_A(obj, rb_path2class("Ray::Text"))) {
    rb_raise(rb_eTypeError, "Can't convert %s into Ray::Text",
             RAY_OBJ_CLASSNAME(obj));
  }

  say_text *text;
  Data_Get_Struct(obj, say_text, text);

  return text;
}

static
VALUE ray_text_alloc(VALUE self) {
  say_text *text = say_text_create();
  return Data_Wrap_Struct(self, NULL, say_text_free, text);
}

/* @return [Ray::Font] */
static
VALUE ray_text_font(VALUE self) {
  return rb_iv_get(self, "@font");
}

/*
  @overload font=(font)
    Sets the text's font.
    @param [Ray::Font] font Font used when drawing
*/
static
VALUE ray_text_set_font(VALUE self, VALUE font) {
  rb_check_frozen(self);
  say_text_set_font(ray_rb2text(self), ray_rb2font(font));
  rb_iv_set(self, "@font", font);
  return font;
}

static
VALUE ray_text_basic_string(VALUE self) {
  say_text *text = ray_rb2text(self);

  uint32_t *str = say_text_get_string(text);
  size_t len = say_text_get_string_length(text);

  if (len == 0)
    return rb_str_new2("");
  else
    return rb_str_new((char*)str, len * 4);
}

static
VALUE ray_text_set_basic_string(VALUE self, VALUE str) {
  rb_check_frozen(self);

  say_text *text = ray_rb2text(self);

  uint32_t *c_str = (uint32_t*)StringValuePtr(str);
  size_t len = RSTRING_LEN(str) / 4;

  say_text_set_string(text, c_str, len);

  return str;
}

/* @return [Integer] Character size in pixels */
static
VALUE ray_text_size(VALUE self) {
  return INT2FIX(say_text_get_size(ray_rb2text(self)));
}

/*
  @overload size=(val)
    @param [Integer] val New charcter size.
*/
static
VALUE ray_text_set_size(VALUE self, VALUE val) {
  rb_check_frozen(self);
  say_text_set_size(ray_rb2text(self), NUM2ULONG(val));
  return val;
}

/*
  @return [Integer] Text style (see constants Normal, Italic, Bold, and
    Underlined)
*/
static
VALUE ray_text_style(VALUE self) {
  return INT2FIX(say_text_get_style(ray_rb2text(self)));
}

/*
  @overload style=(val)
    @param [Integer] val New text style
*/
static
VALUE ray_text_set_style(VALUE self, VALUE val) {
  rb_check_frozen(self);
  say_text_set_style(ray_rb2text(self), NUM2ULONG(val));
  return val;
}

/*
  @return [Color] Text color
*/
static
VALUE ray_text_color(VALUE self) {
  return ray_col2rb(say_text_get_color(ray_rb2text(self)));
}

/*
  @overload color=(val)
    @param [Color] val New text color
*/
static
VALUE ray_text_set_color(VALUE self, VALUE val) {
  rb_check_frozen(self);
  say_text_set_color(ray_rb2text(self), ray_rb2col(val));
  return val;
}

/*
  @return [Rect] Rect occupied by the text
*/
static
VALUE ray_text_rect(VALUE self) {
  return ray_rect2rb(say_text_get_rect(ray_rb2text(self)));
}


void Init_ray_text() {
  ray_cText = rb_define_class_under(ray_mRay, "Text", ray_cDrawable);
  rb_define_alloc_func(ray_cText, ray_text_alloc);

  rb_define_method(ray_cText, "font", ray_text_font, 0);
  rb_define_method(ray_cText, "font=", ray_text_set_font, 1);

  rb_define_private_method(ray_cText, "basic_string", ray_text_basic_string, 0);
  rb_define_private_method(ray_cText, "set_basic_string",
                           ray_text_set_basic_string, 1);

  rb_define_method(ray_cText, "size", ray_text_size, 0);
  rb_define_method(ray_cText, "size=", ray_text_set_size, 1);

  rb_define_method(ray_cText, "style", ray_text_style, 0);
  rb_define_private_method(ray_cText, "set_basic_style", ray_text_set_style, 1);

  rb_define_method(ray_cText, "color", ray_text_color, 0);
  rb_define_method(ray_cText, "color=", ray_text_set_color, 1);

  rb_define_method(ray_cText, "rect", ray_text_rect, 0);

  rb_define_const(ray_cText, "Normal", INT2FIX(SAY_TEXT_NORMAL));
  rb_define_const(ray_cText, "Bold", INT2FIX(SAY_TEXT_BOLD));
  rb_define_const(ray_cText, "Italic", INT2FIX(SAY_TEXT_ITALIC));
  rb_define_const(ray_cText, "Underlined", INT2FIX(SAY_TEXT_UNDERLINED));
}
