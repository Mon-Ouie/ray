#include "ray.h"

VALUE ray_cInput = Qnil;

say_input *ray_rb2input(VALUE obj) {
  if (!RAY_IS_A(obj, rb_path2class("Ray::Input"))) {
    rb_raise(rb_eTypeError, "Can't convert %s into Ray::Input",
             RAY_OBJ_CLASSNAME(obj));
  }

  say_input *input = NULL;
  Data_Get_Struct(obj, say_input, input);

  return input;
}

VALUE ray_input2rb(say_input *input, VALUE owner) {
  VALUE obj = Data_Wrap_Struct(rb_path2class("Ray::Input"), NULL, NULL, input);
  rb_iv_set(obj, "@owner", owner);

  return obj;
}

static
VALUE ray_input_alloc(VALUE self) {
  rb_raise(rb_eRuntimeError, "can't allocate Ray::Input");
  return Qnil;
}

/*
  @overload holding?(key)
    @param [Integer] key A key
    @return [true, false] True if the given key is being held
*/
static
VALUE ray_input_holding(VALUE self, VALUE key) {
  return say_input_is_holding(ray_rb2input(self), NUM2INT(key)) ?
    Qtrue : Qfalse;
}

/* @return [Ray::Vector2] The position of the mouse */
static
VALUE ray_input_mouse_pos(VALUE self) {
  return ray_vector2_to_rb(say_input_get_mouse_pos(ray_rb2input(self)));
}

void Init_ray_input() {
  ray_cInput = rb_define_class_under(ray_mRay, "Input", rb_cObject);
  rb_define_alloc_func(ray_cInput, ray_input_alloc);

  rb_define_method(ray_cInput, "holding?", ray_input_holding, 1);
  rb_define_method(ray_cInput, "mouse_pos", ray_input_mouse_pos, 0);
}
