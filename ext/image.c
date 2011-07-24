#include "ray.h"

VALUE ray_cImage = Qnil;

say_image *ray_rb2image(VALUE obj) {
  if (!RAY_IS_A(obj, rb_path2class("Ray::Image"))) {
    rb_raise(rb_eTypeError, "can't convert %s into Ray::Image",
             RAY_OBJ_CLASSNAME(obj));
  }

  say_image *img = NULL;
  Data_Get_Struct(obj, say_image, img);

  return img;
}

static
VALUE ray_image_alloc(VALUE self) {
  say_image *img = say_image_create();
  return Data_Wrap_Struct(ray_cImage, NULL, say_image_free, img);
}

/*
 * @overload initialize(io)
 *   Creates an image from an IO object
 *   @param [IO, #read] io An object containing the image.
 *   @example
 *     open("test.png") { |io| Ray::Image.new(io) }
 *
 * @overload initialize(filename)
 *   Loads an image from a file
 *   @param [String] filename Name of a file to load the image from.
 *   @example
 *     Ray::Image.new "test.png"
 *
 * @overload initialize(size)
 *   Creates an image from a size
 *   @param [Vector2, #to_vector2] size Size of the image to create
 *   @example
 *     Ray::Image.new [64, 128]
 */
static
VALUE ray_image_init(VALUE self, VALUE arg) {
  say_image *img = ray_rb2image(self);

  if (rb_respond_to(arg, RAY_METH("read"))) {
    arg = rb_funcall(arg, RAY_METH("read"), 0);
    if (!say_image_load_from_memory(img, RSTRING_LEN(arg),
                                    StringValuePtr(arg))) {
      rb_raise(rb_eRuntimeError, "%s", say_error_get_last());
    }
  }
  else if (rb_respond_to(arg, RAY_METH("to_str"))) {
    if (!say_image_load_file(img, StringValuePtr(arg))) {
      rb_raise(rb_eRuntimeError, "%s", say_error_get_last());
    }
  }
  else {
    say_vector2 size = ray_convert_to_vector2(arg);
    if (!say_image_create_with_size(img, size.x, size.y))
      rb_raise(rb_eRuntimeError, "%s", say_error_get_last());
  }

  return self;
}

static
VALUE ray_image_init_copy(VALUE self, VALUE other) {
  say_image *orig = ray_rb2image(other);

  say_image_load_raw(ray_rb2image(self),
                     say_image_get_width(orig), say_image_get_height(orig),
                     say_image_get_buffer(orig));

  return self;
}

/*
 * @overload write_bmp(filename)
 *   Saves the image as a BMP
 */
static
VALUE ray_image_write_bmp(VALUE self, VALUE filename) {
  if (!say_image_write_bmp(ray_rb2image(self), StringValuePtr(filename))) {
    rb_raise(rb_eRuntimeError, "%s", say_error_get_last());
  }

  return self;
}

/*
 * @overload write_png(filename)
 *   Saves the image as a PNG
 */
static
VALUE ray_image_write_png(VALUE self, VALUE filename) {
  if (!say_image_write_png(ray_rb2image(self), StringValuePtr(filename))) {
    rb_raise(rb_eRuntimeError, "%s", say_error_get_last());
  }

  return self;
}

/*
 * @overload write_tga(filename)
 *   Saves the image as a TGA
 */
static
VALUE ray_image_write_tga(VALUE self, VALUE filename) {
  if (!say_image_write_tga(ray_rb2image(self), StringValuePtr(filename))) {
    rb_raise(rb_eRuntimeError, "%s", say_error_get_last());
  }

  return self;
}

/*
 * @overload write(filename)
 *   Saves the image in any format
 *
 *   This will try to guess the format based on the filename extension
 *   (regardless of the case). If Ray can't guess the filename, the image will
 *   be saved anyway using BMP.
 */
static
VALUE ray_image_write(VALUE self, VALUE filename) {
  if (!say_image_write(ray_rb2image(self), StringValuePtr(filename))) {
    rb_raise(rb_eRuntimeError, "%s", say_error_get_last());
  }

  return self;
}

/* @return [Integer] Width of the image in pixels */
static
VALUE ray_image_width(VALUE self) {
  return INT2FIX(say_image_get_width(ray_rb2image(self)));
}

/* @return [Integer] Height of the image in pixels */
static
VALUE ray_image_height(VALUE self) {
  return INT2FIX(say_image_get_height(ray_rb2image(self)));
}

/* @return [Ray::Vector2] Size of the image in pixels */
static
VALUE ray_image_size(VALUE self) {
  return ray_vector2_to_rb(say_image_get_size(ray_rb2image(self)));
}

static
void ray_image_assert_pos(say_image *img, size_t x, size_t y) {
  if (x >= say_image_get_width(img)) {
    rb_raise(rb_eRangeError, "%ld is not in the 0...%ld range",
             x, say_image_get_width(img));
  }
  else if (y >= say_image_get_height(img)) {
    rb_raise(rb_eRangeError, "%ld is not in the 0...%ld range",
             y, say_image_get_height(img));
  }
}

/*
 * @overload [](x, y)
 *   Reads the color of a pixel
 *
 *   @param [Integer] x X position of the pixel
 *   @param [Integer] y Y position of the pixel
 *
 *   @return [Color] Color of the pixel
 */
static
VALUE ray_image_get(VALUE self, VALUE rb_x, VALUE rb_y) {
  say_image *img = ray_rb2image(self);
  size_t x = NUM2ULONG(rb_x), y = NUM2ULONG(rb_y);

  if (x >= say_image_get_width(img) ||
      y >= say_image_get_height(img))
    return Qnil;

  return ray_col2rb(say_image_get(img, x, y));
}

/*
 * @overload []=(x, y, color)
 *   Changes the color of a pixel
 *
 *   @param [Integer] x X position of the pixel
 *   @param [Integer] y Y position of the pixel
 *
 *   @param [Color] color New color of the pixel
 */
static
VALUE ray_image_set(VALUE self, VALUE rb_x, VALUE rb_y, VALUE color) {
  rb_check_frozen(self);

  say_image *img = ray_rb2image(self);
  size_t x = NUM2ULONG(rb_x), y = NUM2ULONG(rb_y);

  ray_image_assert_pos(img, x, y);

  say_image_set(img, x, y, ray_rb2col(color));
  return color;
}

/*
 * @overload tex_rect(rect)
 *   Converts a rect of pixel coordinates to a rect of texture coordinates
 *
 *   Texture coordinates can be useful when manually generating vertices that
 *   need to be textured.
 *
 *   @param [Rect] rect Rect in pixel coordinates
 *   @return [Rect] Rect in texture coordinates
 */
VALUE ray_image_tex_rect(VALUE self, VALUE rect) {
  return ray_rect2rb(say_image_get_tex_rect(ray_rb2image(self),
                                            ray_convert_to_rect(rect)));
}

/*
 * Binds the texture, which will be used as GL_TEXTURE_2D when drawing with
 * OpenGL
 */
VALUE ray_image_bind(VALUE self) {
  say_image_bind(ray_rb2image(self));
  return self;
}

/*
 * @see smooth=
 */
VALUE ray_image_is_smooth(VALUE self) {
  return say_image_is_smooth(ray_rb2image(self)) ? Qtrue : Qfalse;
}

/*
 * @overload smooth=(val)
 *   Enables or disables smoothing
 *
 *   When smoothing is enabled, the color of interpolated pixels is chosen by
 *   taking the average of the four colors that are the nearest to each of
 *   them.
 *
 *   @param [Boolean] val True to enable smoothing
 */
VALUE ray_image_set_smooth(VALUE self, VALUE val) {
  rb_check_frozen(self);
  say_image_set_smooth(ray_rb2image(self), RTEST(val));
  return val;
}

/*
 * Document-class: Ray::Image
 *
 * An image is a bidimensonial array of pixels used to texture rendering. It is
 * not an object you can draw per se. See {Ray::Sprite} or more generically
 * {Ray::Drawable} for this.
 *
 * You can directly access pixels of an image, but an image target
 * ({Ray::ImageTarget}) must be used to be able to draw more complex objects
 * on it.
 *
 * @see Ray::ImageTarget
 * @see Ray::Sprite
 * @see Ray::Drawable
 */
void Init_ray_image() {
  ray_cImage = rb_define_class_under(ray_mRay, "Image", rb_cObject);

  /* @group Creating an image */
  rb_define_alloc_func(ray_cImage, ray_image_alloc);
  rb_define_method(ray_cImage, "initialize", ray_image_init, 1);
  rb_define_method(ray_cImage, "initialize_copy", ray_image_init_copy, 1);
  /* @endgroup */

  /* @group Saving an image */
  rb_define_method(ray_cImage, "write_bmp", ray_image_write_bmp, 1);
  rb_define_method(ray_cImage, "write_png", ray_image_write_png, 1);
  rb_define_method(ray_cImage, "write_tga", ray_image_write_tga, 1);
  rb_define_method(ray_cImage, "write", ray_image_write, 1);
  /* @endgroup */

  /* @group Getting image size */
  rb_define_method(ray_cImage, "width", ray_image_width, 0);
  rb_define_method(ray_cImage, "height", ray_image_height, 0);
  rb_define_method(ray_cImage, "size", ray_image_size, 0);
  /* @endgroup */

  /* @group Pixel-level access */
  rb_define_method(ray_cImage, "[]", ray_image_get, 2);
  rb_define_method(ray_cImage, "[]=", ray_image_set, 3);
  /* @endgroup */

  /* @group Texture parameters */
  rb_define_method(ray_cImage, "smooth?", ray_image_is_smooth, 0);
  rb_define_method(ray_cImage, "smooth=", ray_image_set_smooth, 1);
  /* @endgroup */

  /* @group Coordinate conversions */
  rb_define_method(ray_cImage, "tex_rect", ray_image_tex_rect, 1);
  /* @endgroup */

  /* @group OpenGL access */
  rb_define_method(ray_cImage, "bind", ray_image_bind, 0);
  /* @endgroup */
}
