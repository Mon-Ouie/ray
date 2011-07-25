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
 * @return [Ray::Vector2] Size of the target, in pixels.
 */
static
VALUE ray_target_size(VALUE self) {
  return ray_vector2_to_rb(say_target_get_size(ray_rb2target(self)));
}

/* @see view= */
static
VALUE ray_target_view(VALUE self) {
  return ray_view2rb(say_target_get_view(ray_rb2target(self)));
}

/*
 * @overload view=(view)
 *   Sets the view used by the target
 *
 *   Notice the view returned by {#view} is a copy of the view used internally.
 *
 *   @param [Ray::View] view New view
 *   @see Ray::View
 */
static
VALUE ray_target_set_view(VALUE self, VALUE val) {
  rb_check_frozen(self);
  say_target_set_view(ray_rb2target(self), ray_rb2view(val));
  return val;
}


/*
 * Default view of the target
 *
 * The default view is made so that no scaling will be performed. It has the
 * following properties:
 *
 * 1. +size+ is the size of the target;
 * 2. +center+ is the center of the target (half of the target);
 * 3. +viewport+ is the whole target (that is, +[0, 0, 1, 1]+).
 *
 * @return [Ray::View] Default view of the target
 */
static
VALUE ray_target_default_view(VALUE self) {
  say_view *view = say_target_get_default_view(ray_rb2target(self));
  return Data_Wrap_Struct(rb_path2class("Ray::View"), NULL, say_view_free,
                          view);
}

/*
 * Low-level access to the shader. {#shader} will cache this.
 * @return [Ray::Shader] The target's shader
 */
static
VALUE ray_target_shader(VALUE self) {
  return ray_shader2rb(say_target_get_shader(ray_rb2target(self)), self);
}

/*
 * Clipping rectangle when using the current view
 * @return [Ray::Rect] Part of the target that's used by the view
 *
 * @see Ray::View.clip
 */
static
VALUE ray_target_clip(VALUE self) {
  return ray_rect2rb(say_target_get_clip(ray_rb2target(self)));
}

/*
 * @overload viewport_for(rect)
 *   Viewport for a view to make it draw on a given rect
 *
 *   @param [Ray::Rect] rect Rect in pixel
 *   @return [Ray::Rect] Viewport to set in the view
 */
static
VALUE ray_target_viewport_for(VALUE self, VALUE rect) {
  return ray_rect2rb(say_target_get_viewport_for(ray_rb2target(self),
                                                 ray_convert_to_rect(rect)));
}

/*
 * Makes a target become the current one
 *
 * This method is only used when using low-level OpenGL access. Calling {#draw}
 * will cause this method to be called anyway (and custom drawables also allow
 * OpenGL calls)
 *
 * @raise If a context could not be created or made current. This Would often
 *   mean the target was not initialized properly (e.g. window is not open)
 */
static
VALUE ray_target_make_current(VALUE self) {
  if (!say_target_make_current(ray_rb2target(self)))
    rb_raise(rb_eRuntimeError, "%s", say_error_get_last());
  return self;
}

/*
 * @overload clear(color)
 *   Clears the target in a given color
 *
 *   This method should usually be called before performing drawing.
 *
 *   @param [Ray::Color] color Color to clear the target with.
 */
static
VALUE ray_target_clear(VALUE self, VALUE color) {
  say_target_clear(ray_rb2target(self), ray_rb2col(color));
  return self;
}

/*
 * @overload draw(obj)
 *   Draws an object on the target
 *   @param [Ray::Drawable, Ray::BufferRenderer] obj Object to be drawn
 */
static
VALUE ray_target_draw(VALUE self, VALUE obj) {
  if (RAY_IS_A(obj, rb_path2class("Ray::BufferRenderer"))) {
    say_target_draw_buffer(ray_rb2target(self),
                           ray_rb2buf_renderer(obj));
  }
  else
    say_target_draw(ray_rb2target(self), ray_rb2drawable(obj));
  return self;
}

/*
 * @overload [](x, y)
 *  Color of the pixel at a given position
 *
 *  This operation is slow. It is much faster to access a rect or the whole
 *  image than to read each pixel from that rect.
 *
 *  @param [Integer] x
 *  @param [Integer] y
 *
 *  @return [Ray::Color] Color of the pixel at that position
 */
static
VALUE ray_target_get(VALUE self, VALUE x, VALUE y) {
  return ray_col2rb(say_target_get(ray_rb2target(self),
                                   NUM2ULONG(x),
                                   NUM2ULONG(y)));
}

/*
 * @overload rect(rect)
 *   Colors of pixels present in a rect
 *   @param [Ray::Rect] rect Rect to read pixels from
 *   @return [Ray::Image] An image containing the pixels of that rect.
 */
static
VALUE ray_target_rect(VALUE self, VALUE rect) {
  say_rect c_rect  = ray_convert_to_rect(rect);
  say_image *image = say_target_get_rect(ray_rb2target(self),
                                         c_rect.x, c_rect.y,
                                         c_rect.w, c_rect.h);

  if (!image) {
    rb_raise(rb_eRuntimeError, "%s", say_error_get_last());
  }

  return Data_Wrap_Struct(rb_path2class("Ray::Image"), NULL, say_image_free,
                          image);
}

/*
 * Colors of all the pixels in the target
 * @return [Ray::Image] An image containing all the pixels of the target
 */
static
VALUE ray_target_to_image(VALUE self) {
  say_image *image = say_target_to_image(ray_rb2target(self));

  if (!image) {
    rb_raise(rb_eRuntimeError, "%s", say_error_get_last());
  }

  return Data_Wrap_Struct(rb_path2class("Ray::Image"), NULL, say_image_free,
                          image);
}

void Init_ray_target() {
  ray_cTarget = rb_define_class_under(ray_mRay, "Target", rb_cObject);

  rb_define_method(ray_cTarget, "size", ray_target_size, 0);

  /* @group Manipulating views */
  rb_define_method(ray_cTarget, "view", ray_target_view, 0);
  rb_define_method(ray_cTarget, "view=", ray_target_set_view, 1);
  rb_define_method(ray_cTarget, "default_view", ray_target_default_view, 0);

  rb_define_method(ray_cTarget, "clip", ray_target_clip, 0);
  rb_define_method(ray_cTarget, "viewport_for", ray_target_viewport_for, 1);
  /* @endgroup */

  rb_define_private_method(ray_cTarget, "simple_shader", ray_target_shader, 0);

  /* @group Low-level access */
  rb_define_method(ray_cTarget, "make_current", ray_target_make_current, 0);
  /* @endgroup */

  /* @group Drawing */
  rb_define_method(ray_cTarget, "clear", ray_target_clear, 1);
  rb_define_method(ray_cTarget, "draw", ray_target_draw, 1);
  /* @endgroup */

  /* @group Pixel-level access */
  rb_define_method(ray_cTarget, "[]", ray_target_get, 2);
  rb_define_method(ray_cTarget, "rect", ray_target_rect, 1);
  rb_define_method(ray_cTarget, "to_image", ray_target_to_image, 0);
  /* @endgroup */
}
