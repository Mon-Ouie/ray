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
  say_image_target *target = say_image_target_create();
  return Data_Wrap_Struct(ray_cImageTarget, NULL, say_image_target_free,
                          target);
}

/* @overload image=(img) */
VALUE ray_image_target_set_image(VALUE self, VALUE img) {
  rb_check_frozen(self);
  say_image_target_set_image(ray_rb2image_target(self), ray_rb2image(img));
  rb_iv_set(self, "@image", img);
  return img;
}

/* @return [Ray::Image] image the target draws on */
VALUE ray_image_target_image(VALUE self) {
  return rb_iv_get(self, "@image");
}

/* Updates the content of the image target */
VALUE ray_image_target_update(VALUE self) {
  say_image_target_update(ray_rb2image_target(self));
  return self;
}

/* Binds an image target to draw directly on it */
VALUE ray_image_target_bind(VALUE self) {
  say_image_target_bind(ray_rb2image_target(self));
  return self;
}

/*
 * Unbinds any image target. This is mostly not needed, as making a window the
 * current target will call this automatically.
 */
VALUE ray_image_target_unbind(VALUE self) {
  say_image_target_unbind();
  return Qnil;
}

void Init_ray_image_target() {
  ray_cImageTarget = rb_define_class_under(ray_mRay, "ImageTarget", ray_cTarget);
  rb_define_alloc_func(ray_cImageTarget, ray_image_target_alloc);

  rb_define_singleton_method(ray_cImageTarget, "unbind", ray_image_target_unbind, 0);

  rb_define_method(ray_cImageTarget, "image=", ray_image_target_set_image, 1);
  rb_define_method(ray_cImageTarget, "image", ray_image_target_image, 0);

  rb_define_method(ray_cImageTarget, "bind", ray_image_target_bind, 0);
  rb_define_method(ray_cImageTarget, "update", ray_image_target_update, 0);
}
