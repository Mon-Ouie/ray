#include "ray.h"

VALUE ray_cImageTarget = Qnil;

say_image_target *ray_rb2image_target(VALUE obj) {
  if (!RAY_IS_A(obj, rb_path2class("Ray::ImageTarget"))) {
    rb_raise(rb_eTypeError, "Can't convert %s into Ray::ImageTarget",
             RAY_OBJ_CLASSNAME(obj));
  }

  say_image_target *ret = NULL;
  Data_Get_Struct(obj, say_image_target, ret);

  return ret;
}

VALUE ray_image_target_alloc(VALUE self) {
  if (!say_image_target_is_available())
    rb_raise(rb_eRuntimeError, "Ray::RenderImage is not supported here");

  say_image_target *target = say_image_target_create();
  return Data_Wrap_Struct(ray_cImageTarget, NULL, say_image_target_free,
                          target);
}

/*
 * @overload image=(img)
 *   Sets the image this object will draw on
 *   @param [Ray::Image] img New image to draw on
 */
VALUE ray_image_target_set_image(VALUE self, VALUE img) {
  rb_check_frozen(self);
  say_image_target_set_image(ray_rb2image_target(self), ray_rb2image(img));
  rb_iv_set(self, "@image", img);
  return img;
}

/* @see image= */
VALUE ray_image_target_image(VALUE self) {
  return rb_iv_get(self, "@image");
}

/*
 * Updates the content of the image target
 *
 * The pixels of the image will also be updated:
 *    Ray::ImageTarget.new image do |target|
 *      target.clear Ray::Color.red
 *      target.update
 *    end
 *
 *    image[0, 0] # => RGBA(255, 0, 0, 255)
 */
VALUE ray_image_target_update(VALUE self) {
  say_image_target_update(ray_rb2image_target(self));
  return self;
}

/*
 * Binds an image target to draw directly on it
 *
 * This method is only useful when performing low-level OpenGL rendering. It
 * is different from {#make_current} because it doesn't use the target's own
 * OpenGL context. Instead, it will use the current context (ensuring there is
 * one).
 *
 * Notice binding the image target (and, therefore, making it the current
 * target) clears the depth buffer of the image, so that everything will be
 * rendered over what was already on the image.
 */
VALUE ray_image_target_bind(VALUE self) {
  say_image_target_bind(ray_rb2image_target(self));
  return self;
}

/*
 * Unbinds any image target
 *
 * This is rarely needed, as this method gets called automatically when a window
 * is bound, and because windows and image targets do not share the same OpenGL
 * context.
 */
VALUE ray_image_target_unbind(VALUE self) {
  /*
   * No need for an error when this is not supported, because it just means
   * we don't need to unbind anything.
   */
  if (say_image_target_is_available())
    say_image_target_unbind();
  return Qnil;
}

/*
 * Checks availability of image targets
 *
 * Image targets are only part of OpenGL core since OpenGL 3 (although they may
 * be supported as an extension in older versions).
 *
 * @return [true, false] True when ImageTargets are available
 */
VALUE ray_image_target_available(VALUE self) {
  return say_image_target_is_available() ? Qtrue : Qfalse;
}

/*
 * Document-class: Ray::ImageTarget
 *
 * Image targets are objects that allow to draw any drawable object on an image
 * instead of doing on-screen rendering. Off-screen rendering can be useful to
 * pre-render some objects.
 *
 * Notice image targets modify the image they are drawing on directly. You may
 * therefore not want to draw on a cached image, but rather on a copy thereof.
 *
 * @see Ray::Image
 */
void Init_ray_image_target() {
  ray_cImageTarget = rb_define_class_under(ray_mRay, "ImageTarget",
                                           ray_cTarget);
  rb_define_alloc_func(ray_cImageTarget, ray_image_target_alloc);

  rb_define_singleton_method(ray_cImageTarget, "unbind",
                             ray_image_target_unbind, 0);
  rb_define_singleton_method(ray_cImageTarget, "available?",
                             ray_image_target_available, 0);

  rb_define_method(ray_cImageTarget, "image=", ray_image_target_set_image, 1);
  rb_define_method(ray_cImageTarget, "image", ray_image_target_image, 0);

  rb_define_method(ray_cImageTarget, "bind", ray_image_target_bind, 0);
  rb_define_method(ray_cImageTarget, "update", ray_image_target_update, 0);
}
