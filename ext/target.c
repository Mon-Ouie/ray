#include "ray.h"

VALUE ray_cTarget = Qnil;

say_target *ray_rb2target(VALUE obj) {
  if (RAY_IS_A(obj, rb_path2class("Ray::Window"))) {
    return ray_rb2window(obj)->target;
  }
  else if (RAY_IS_A(obj, rb_path2class("Ray::ImageTarget"))) {
    return ray_rb2image_target(obj)->target;
  }
  else {
    rb_raise(rb_eTypeError, "can't get target pointer from %s",
             RAY_OBJ_CLASSNAME(obj));
  }

  return NULL;
}

/*
  @return [Ray::Vector2] Size of the target, in pixels.
*/
static
VALUE ray_target_size(VALUE self) {
  return ray_vector2_to_rb(say_target_get_size(ray_rb2target(self)));
}

/* @return [Ray::View] View currently used */
static
VALUE ray_target_view(VALUE self) {
  return ray_view2rb(say_target_get_view(ray_rb2target(self)));
}

static
VALUE ray_target_set_view(VALUE self, VALUE val) {
  rb_check_frozen(self);
  say_target_set_view(ray_rb2target(self), ray_rb2view(val));
  return val;
}


/* @return [Ray::View] Default view of the target */
static
VALUE ray_target_default_view(VALUE self) {
  say_view *view = say_target_get_default_view(ray_rb2target(self));
  return Data_Wrap_Struct(rb_path2class("Ray::View"), NULL, say_view_free, view);
}

static
VALUE ray_target_shader(VALUE self) {
  return ray_shader2rb(say_target_get_shader(ray_rb2target(self)), self);
}

/* @return [Ray::Rect] Part of the target that's used by the view */
static
VALUE ray_target_clip(VALUE self) {
  return ray_rect2rb(say_target_get_clip(ray_rb2target(self)));
}

/*
  @overload viewport_for(rect)
    @param [Ray::Rect] rect Rect in pixel
    @return [Ray::Rect] Viewport to set in the view
 */
static
VALUE ray_target_viewport_for(VALUE self, VALUE rect) {
  return ray_rect2rb(say_target_get_viewport_for(ray_rb2target(self),
                                                 ray_convert_to_rect(rect)));
}

/*
  Makes a target become the current one.
  Not useful unless using OpenGL directly.

  @raise If a context could not be created or made current. Would often mean the
    target was not initialized properly (e.g. window is not open)
*/
static
VALUE ray_target_make_current(VALUE self) {
  if (!say_target_make_current(ray_rb2target(self)))
    rb_raise(rb_eRuntimeError, "%s", say_error_get_last());
  return self;
}

/*
  @overload clear(color)
    Clears the target in a given color. Should usually be called before drawing
    anything in a new frame.

    @param [Ray::Color] color Color to clear the target with.
*/
static
VALUE ray_target_clear(VALUE self, VALUE color) {
  say_target_clear(ray_rb2target(self), ray_rb2col(color));
  return self;
}

/*
  @overload draw(obj)
    Draws an object on the target.
    @param [Ray::Drawable, #to_drawable] obj Object to be drawn
*/
static
VALUE ray_target_draw(VALUE self, VALUE obj) {
  say_target_draw(ray_rb2target(self), ray_rb2drawable(obj));
  return self;
}

void Init_ray_target() {
  ray_cTarget = rb_define_class_under(ray_mRay, "Target", rb_cObject);

  rb_define_method(ray_cTarget, "size", ray_target_size, 0);

  rb_define_method(ray_cTarget, "view", ray_target_view, 0);
  rb_define_method(ray_cTarget, "view=", ray_target_set_view, 1);
  rb_define_method(ray_cTarget, "default_view", ray_target_default_view, 0);

  rb_define_private_method(ray_cTarget, "simple_shader", ray_target_shader, 0);

  rb_define_method(ray_cTarget, "clip", ray_target_clip, 0);
  rb_define_method(ray_cTarget, "viewport_for", ray_target_viewport_for, 1);

  rb_define_method(ray_cTarget, "make_current", ray_target_make_current, 0);
  rb_define_method(ray_cTarget, "clear", ray_target_clear, 1);
  rb_define_method(ray_cTarget, "draw", ray_target_draw, 1);
}
