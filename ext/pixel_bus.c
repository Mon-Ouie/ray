#include "ray.h"

VALUE ray_cPixelBus = Qnil;

say_pixel_bus *ray_rb2pixel_bus(VALUE obj) {
  if (!rb_obj_is_kind_of(obj, rb_path2class("Ray::PixelBus"))) {
    rb_raise(rb_eTypeError, "can't convert %s into Ray::PixelBus",
             RAY_OBJ_CLASSNAME(obj));
  }

  say_pixel_bus **ptr = NULL;
  Data_Get_Struct(obj, say_pixel_bus*, ptr);

  if (!*ptr)
    rb_raise(rb_eRuntimeError, "trying to use uninitialized pixel bus");

  return *ptr;
}

static
void ray_pixel_bus_free(say_pixel_bus **ptr) {
  if (*ptr) say_pixel_bus_free(*ptr);
}

static
VALUE ray_pixel_bus_alloc(VALUE self) {
  say_pixel_bus **obj = malloc(sizeof(say_pixel_bus*));
  *obj = NULL;

  return Data_Wrap_Struct(self, NULL, ray_pixel_bus_free, obj);
}

/*
 * @overload initialize(type = :stream)
 *   @param type (see Ray::BufferRenderer#initialize)
 */
static
VALUE ray_pixel_bus_init(int argc, VALUE *argv, VALUE self) {
  if (!say_pixel_bus_is_available())
    rb_raise(rb_eRuntimeError, "pixel buses aren't available on this platform");

  GLenum mode = SAY_STREAM;

  VALUE rb_mode = Qnil;
  rb_scan_args(argc, argv, "01", &rb_mode);

  if (!NIL_P(rb_mode)) mode = ray_buf_type(rb_mode);

  say_pixel_bus **ptr = NULL;
  Data_Get_Struct(self, say_pixel_bus*, ptr);

  *ptr = say_pixel_bus_create(mode);

  return self;
}

/*
 * @return [Boolean] True if pixel buses are available on this platform
 */
static
VALUE ray_pixel_bus_available(VALUE self) {
  return say_pixel_bus_is_available() ? Qtrue : Qfalse;
}

/*
 * @return [Integer] The capacity of the pixel bus, in pixels
 */
static
VALUE ray_pixel_bus_size(VALUE self) {
  return ULONG2NUM(say_pixel_bus_get_size(ray_rb2pixel_bus(self)));
}

/*
 * @overload resize(new_size)
 *   Changes the capacity of the buffer
 *
 *   This method needs to copy the buffers content onto client memory, and is
 *   thus signinficantly slower than {#resize!}. It should not be called if the
 *   current content of the buffer doesn't need to be read anymore.
 *
 *   @param [Integer] new_size New capacity of the buffer
 *   @see PixelBus#resize!
 */
static
VALUE ray_pixel_bus_resize(VALUE self, VALUE size) {
  say_pixel_bus_resize(ray_rb2pixel_bus(self), NUM2ULONG(size));
  return self;
}

/*
 * @overload resize!(new_size)
 *   Changes the capacity of the buffer without preserving content
 *
 *   This method allows to quickly resize the buffer used by the pixel bus. The
 *   content of the buffer is however undefined after this call.
 *
 *   @param new_size (see #resize)
 *   @see PixelBus#resize
 */
static
VALUE ray_pixel_bus_resize_fast(VALUE self, VALUE size) {
  say_pixel_bus_resize_fast(ray_rb2pixel_bus(self), NUM2ULONG(size));
  return self;
}

/*
 * @overload pull(object, opts = {})
 *   Pulls data from an object
 *
 *   This method reads a rectangle from an image on a target and stores it in
 *   the buffer, at a given offset. A rect can optionally be specified for
 *   targets. Otherwise, the whole object will be copied.
 *
 *   @param [Ray::Image, Ray::Target] object Object to read pixels from
 *   @option opts [Integer] :offset (0) Offset from the buffer
 *   @option opts [Ray::Rect, nil] :rect Rect of pixels to read for targets
 *
 *   @raise [RangeError] If this method attempts to write outside of the buffer
 *     boundaries.
 *
 *   @return [Integer] Offset of the last read element, plus one.
 */
static
VALUE ray_pixel_bus_pull(int argc, VALUE *argv, VALUE self) {
  say_pixel_bus *bus = ray_rb2pixel_bus(self);

  VALUE object, opts = Qnil;
  rb_scan_args(argc, argv, "11", &object, &opts);

  say_rect rect   = say_make_rect(0, 0, 0, 0);
  size_t   offset = 0;

  say_target *target = NULL;
  say_image  *image  = NULL;

  if (RAY_IS_A(object, rb_path2class("Ray::Target")))
    target = ray_rb2target(object);
  else
    image = ray_rb2image(object);

  say_vector2 size;

  if (target)
    size = say_target_get_size(target);
  else
    size = say_image_get_size(image);

  if (!NIL_P(opts)) {
    if (!RAY_IS_A(opts, rb_cHash)) {
      rb_raise(rb_eTypeError, "can't convert %s into Hash",
               RAY_OBJ_CLASSNAME(opts));
    }

    VALUE rb_rect   = rb_hash_aref(opts, RAY_SYM("rect"));
    VALUE rb_offset = rb_hash_aref(opts, RAY_SYM("offset"));

    if (target) {
      if (!NIL_P(rb_rect))
        rect = ray_convert_to_rect(rb_rect);
      else
        rect = say_make_rect(0, 0, size.x, size.y);
    }

    if (!NIL_P(rb_offset))
      offset = NUM2ULONG(rb_offset);
  }
  else if (target) {
    say_vector2 size = say_target_get_size(target);
    rect = say_make_rect(0, 0, size.x, size.y);
  }

  size_t ret = offset + (size_t)size.x * (size_t)size.y;
  if (ret > say_pixel_bus_get_size(bus))
    rb_raise(rb_eRangeError, "trying to write outside of buffer boundaries");

  if (target) {
    say_pixel_bus_pull_target(bus, target, offset,
                              rect.x, rect.y, rect.w, rect.h);
  }
  else {
    say_pixel_bus_pull_image(bus, image, offset);
  }

  return ULONG2NUM(ret);
}

/*
 * @overload push(image, opts = {})
 *   Pushes data from the buffer onto an image
 *
 *   This method reads pixels from the buffer at a given offset, and copies them
 *   onto an image. A rect of pixels that must be replaced can be
 *   specifed. Otherwise, the whole texture will be replaced.
 *
 *   @param [Ray::Image] image Image to copy the rect to
 *   @option opts :offset (see #pull)
 *   @option opts [Ray::Rect] :rect Rect of pixels to replace in the image
 *
 *   @return [Ray::PixelBus] self
 *
 *   @raise [RangeError] If this method tries to read pixels outside of the
 *     buffer boundaries or to write oustide of the image boundaries.
 */
static
VALUE ray_pixel_bus_push(int argc, VALUE *argv, VALUE self) {
  say_pixel_bus *bus = ray_rb2pixel_bus(self);

  VALUE object, opts = Qnil;
  rb_scan_args(argc, argv, "11", &object, &opts);

  say_image *image = ray_rb2image(object);

  say_vector2 size = say_image_get_size(image);

  size_t offset = 0;
  say_rect rect = say_make_rect(0, 0, size.x, size.y);

  if (!NIL_P(opts)) {
    if (!RAY_IS_A(opts, rb_cHash)) {
      rb_raise(rb_eTypeError, "can't convert %s into Hash",
               RAY_OBJ_CLASSNAME(opts));
    }

    VALUE rb_rect   = rb_hash_aref(opts, RAY_SYM("rect"));
    VALUE rb_offset = rb_hash_aref(opts, RAY_SYM("offset"));

    if (!NIL_P(rb_rect))
      rect = ray_convert_to_rect(rb_rect);

    if (!NIL_P(rb_offset))
      offset = NUM2ULONG(rb_offset);
  }

  size_t ret = offset + ((size_t)rect.w * (size_t)rect.h);
  if (ret > say_pixel_bus_get_size(bus))
    rb_raise(rb_eRangeError, "trying to read outside of pixel bus boundaries");

  if (rect.x + rect.w > size.x || rect.y + rect.h > size.y)
    rb_raise(rb_eRangeError, "trying to write outsied of image boundaries");

  say_pixel_bus_push_image(bus, image, offset, rect.x, rect.y, rect.w, rect.h);
  return self;
}

/*
 * Binds the pixel bus for packing and unpacking
 */
static
VALUE ray_pixel_bus_bind(VALUE self) {
  say_pixel_bus_bind(ray_rb2pixel_bus(self));
  return self;
}

/*
 * Binds the pixel bus for packing
 */
static
VALUE ray_pixel_bus_bind_pack(VALUE self) {
  say_pixel_bus_bind_pack(ray_rb2pixel_bus(self));
  return self;
}

/*
 * Binds the pixel bus for unpacking
 */
static
VALUE ray_pixel_bus_bind_unpack(VALUE self) {
  say_pixel_bus_bind_unpack(ray_rb2pixel_bus(self));
  return self;
}

/*
 * Unbinds any pixel bus bound for either packing or unpacking
 */
static
VALUE ray_pixel_bus_unbind(VALUE self) {
  say_pixel_bus_unbind();
  return Qnil;
}

/*
 * Unbinds any pixel bus bound for packing
 */
static
VALUE ray_pixel_bus_unbind_pack(VALUE self) {
  say_pixel_bus_unbind_pack();
  return Qnil;
}

/*
 * Unbinds any pixel bus bound for unpacking
 */
static
VALUE ray_pixel_bus_unbind_unpack(VALUE self) {
  say_pixel_bus_unbind_unpack();
  return Qnil;
}
/*
 * Document-class: Ray::PixelBus
 *
 * Pixel buses are a way to copy pixels from a target or an image onto an image
 * without having to copy them to client memory, and possibly
 * asynchronously. They may unfortunately not be available on some platforms.
 *
 * Notice that pixels are stored from bottom to top in the buffer. You may have
 * to rely on this when computing offsets manually.
 */
void Init_ray_pixel_bus() {
  ray_cPixelBus = rb_define_class_under(ray_mRay, "PixelBus", rb_cObject);

  rb_define_alloc_func(ray_cPixelBus, ray_pixel_bus_alloc);
  rb_define_method(ray_cPixelBus, "initialize", ray_pixel_bus_init, -1);

  rb_define_singleton_method(ray_cPixelBus, "available?",
                             ray_pixel_bus_available, 0);

  /* @group Manipulating buffer size */
  rb_define_method(ray_cPixelBus, "size", ray_pixel_bus_size, 0);
  rb_define_method(ray_cPixelBus, "resize", ray_pixel_bus_resize, 1);
  rb_define_method(ray_cPixelBus, "resize!", ray_pixel_bus_resize_fast, 1);
  /* @endgroup */

  /* @group Copying data */
  rb_define_method(ray_cPixelBus, "pull", ray_pixel_bus_pull, -1);
  rb_define_method(ray_cPixelBus, "push", ray_pixel_bus_push, -1);
  /* @endgroup */

  /* @group OpenGL access */
  rb_define_method(ray_cPixelBus, "bind", ray_pixel_bus_bind, 0);
  rb_define_method(ray_cPixelBus, "bind_pack", ray_pixel_bus_bind_pack, 0);
  rb_define_method(ray_cPixelBus, "bind_unpack", ray_pixel_bus_bind_unpack, 0);

  rb_define_singleton_method(ray_cPixelBus, "unbind", ray_pixel_bus_unbind, 0);
  rb_define_singleton_method(ray_cPixelBus, "unbind_pack",
                             ray_pixel_bus_unbind_pack, 0);
  rb_define_singleton_method(ray_cPixelBus, "unbind_unpack",
                             ray_pixel_bus_unbind_unpack, 0);
  /* @endgroup */
}
