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
  say_input *obj = malloc(sizeof(say_input));
  say_input_reset(obj);
  return Data_Wrap_Struct(self, NULL, free, obj);
}

/*
 * Resets the input
 *
 * After a reset, all the keys are marked released and the mouse position is set
 * to (0, 0).
 */
static
VALUE ray_input_reset(VALUE self) {
  say_input_reset(ray_rb2input(self));
  return self;
}

/*
 * @overload press(key)
 *   Marks a key as pressed
 *   @param [Integer] key A key
 */
static
VALUE ray_input_press(VALUE self, VALUE key) {
  say_input_press(ray_rb2input(self), NUM2INT(key));
  return Qnil;
}

/*
 * @overload release(key)
 *   Marks a key as released
 *   @param [Integer] key A key
 */
static
VALUE ray_input_release(VALUE self, VALUE key) {
  say_input_release(ray_rb2input(self), NUM2INT(key));
  return Qnil;
}

/*
 * @overload mouse_pos=(pos)
 *   Changes the known position of the mouse
 *
 *   Notice this only affects the input object's state, it won't actually move
 *   the mouse.
 *
 *   @param [Ray::Vector2] pos New position
 */
static
VALUE ray_input_set_mouse_pos(VALUE self, VALUE pos) {
  say_input_set_mouse_pos(ray_rb2input(self), ray_convert_to_vector2(pos));
  return pos;
}

/*
 * @overload holding?(key)
 *   Checks if a key is being held
 *
 *   @param [Integer] key A key
 *   @return [true, false] True if the given key is being held
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

/*
 * Document-class: Ray::Input
 *
 * An input object is used to keep track of pressed keys and the position of the
 * mouse. Methods are also provided to affect the state of the input, possibly
 * useful in tests.
 */
void Init_ray_input() {
  ray_cInput = rb_define_class_under(ray_mRay, "Input", rb_cObject);
  rb_define_alloc_func(ray_cInput, ray_input_alloc);

  /* @group Change input */
  rb_define_method(ray_cInput, "reset", ray_input_reset, 0);
  rb_define_method(ray_cInput, "press", ray_input_press, 1);
  rb_define_method(ray_cInput, "release", ray_input_release, 1);
  rb_define_method(ray_cInput, "mouse_pos=", ray_input_set_mouse_pos, 1);
  /* @engroup */

  /* @group Read input */
  rb_define_method(ray_cInput, "holding?", ray_input_holding, 1);
  rb_define_method(ray_cInput, "mouse_pos", ray_input_mouse_pos, 0);
  /* @endgroup */
}
