#include "ray.h"

VALUE ray_cSprite = Qnil;

say_sprite *ray_rb2sprite(VALUE obj) {
  if (!RAY_IS_A(obj, rb_path2class("Ray::Sprite"))) {
    rb_raise(rb_eTypeError, "Can't convert %s into Ray::Sprite",
             RAY_OBJ_CLASSNAME(obj));
  }

  say_sprite *sprite;
  Data_Get_Struct(obj, say_sprite, sprite);

  return sprite;
}

VALUE ray_sprite_alloc(VALUE self) {
  say_sprite *sprite = say_sprite_create();
  return Data_Wrap_Struct(self, NULL, say_sprite_free, sprite);
}

/*
  @overload image=(img)
    @param [Ray::Image, nil] img The image this sprite will use. No image means
      it will just use the default one.
*/
static
VALUE ray_sprite_set_image(VALUE self, VALUE img) {
  rb_check_frozen(self);
  say_sprite_set_image(ray_rb2sprite(self),
                       NIL_P(img) ? NULL : ray_rb2image(img));
  rb_iv_set(self, "@image", img);
  return self;
}

/*
  @overload sub_rect=(rect)
    @param [Ray::Rect] rect Sets the part of the image the sprite will show. It
      is defaulted to the size of the image.
*/
static
VALUE ray_sprite_set_sub_rect(VALUE self, VALUE rect) {
  rb_check_frozen(self);
  say_sprite_set_rect(ray_rb2sprite(self), ray_convert_to_rect(rect));
  return rect;
}

/*
  @overload color=(col)
    @param [Ray::Color] col Color of the sprite. The color of each pixel will be
      multiplied by this color. The defaul is white (which means the color of
      the image isn't changed).
*/
static
VALUE ray_sprite_set_color(VALUE self, VALUE color) {
  rb_check_frozen(self);
  say_sprite_set_color(ray_rb2sprite(self), ray_rb2col(color));
  return color;
}

/*
  @overload flip_x=(val)
    @param [true, false] val True to flip the sprite horizontally.
*/
static
VALUE ray_sprite_set_flip_x(VALUE self, VALUE val) {
  rb_check_frozen(self);
  say_sprite_flip_x(ray_rb2sprite(self), RTEST(val));
  return val;
}

/*
  @overload flip_y=(val)
    @param [true, false] val True to flip the sprite vertically.
*/
static
VALUE ray_sprite_set_flip_y(VALUE self, VALUE val) {
  rb_check_frozen(self);
  say_sprite_flip_y(ray_rb2sprite(self), RTEST(val));
  return val;
}

/* @return [Ray::Image, nil] The image used by this sprite */
static
VALUE ray_sprite_image(VALUE self) {
  return rb_iv_get(self, "@image");
}

/* @return [Ray::Rect] The part of the image shown by the sprite */
static
VALUE ray_sprite_sub_rect(VALUE self) {
  return ray_rect2rb(say_sprite_get_rect(ray_rb2sprite(self)));
}

/*
  @return [Ray::Color]
  @see #color=
*/
static
VALUE ray_sprite_color(VALUE self) {
  return ray_col2rb(say_sprite_get_color(ray_rb2sprite(self)));
}

/* @return [true, false] True if the sprite is horizontally flipped */
static
VALUE ray_sprite_x_flipped(VALUE self) {
  return say_sprite_is_x_flipped(ray_rb2sprite(self)) ? Qtrue : Qfalse;
}

/* @return [true, false] True if the sprite is vertically flipped */
static
VALUE ray_sprite_y_flipped(VALUE self) {
  return say_sprite_is_y_flipped(ray_rb2sprite(self)) ? Qtrue : Qfalse;
}

/*
  @overload sheet_size=(size)

  Sets the size of the sprite sheet. For instance,
    sprite.sheet_size = [3, 4]
  would mean there are 4 rows and 3 columns in the sprite (and each cell
  has the same size).
*/
static
VALUE ray_sprite_set_sheet_size(VALUE self, VALUE size) {
  rb_check_frozen(size);
  say_sprite_set_sheet_size(ray_rb2sprite(self), ray_convert_to_vector2(size));
  return size;
}

/* @return [Ray::Vector2, nil] size of the sprite sheet */
static
VALUE ray_sprite_sheet_size(VALUE self) {
  say_sprite *sprite = ray_rb2sprite(self);
  if (say_sprite_uses_sprite_sheet(sprite))
    return ray_vector2_to_rb(say_sprite_get_sheet_size(sprite));
  else
    return Qnil;
}

/*
  @overload sheet_pos=(pos)

  Sets which cell of the sprite sheet should be displayed.
  sprite.sheet_pos = [0, 1] # Uses the first cell of the second line.

  pos.x and pos.y are rounded to floor.
  Passing a too high value will make the sprite use the previous cells.

    sprite.sheet_size = [4, 4]

    sprite.sheet_pos = [5, 5]
    sprite.sheet_pos == [1, 1] # => true
*/
static
VALUE ray_sprite_set_sheet_pos(VALUE self, VALUE size) {
  rb_check_frozen(size);

  say_sprite *sprite = ray_rb2sprite(self);

  if (say_sprite_uses_sprite_sheet(sprite))
    say_sprite_set_sheet_pos(ray_rb2sprite(self), ray_convert_to_vector2(size));
  else
    rb_raise(rb_eRuntimeError, "sprite sheet not enabled on this sprite");

  return size;
}

/* @return [Ray::Vector2, nil] Position in the sprite sheet */
static
VALUE ray_sprite_sheet_pos(VALUE self) {
  say_sprite *sprite = ray_rb2sprite(self);
  if (say_sprite_uses_sprite_sheet(sprite))
    return ray_vector2_to_rb(say_sprite_get_sheet_pos(sprite));
  else
    return Qnil;
}

/* @return [Float, nil] width of a cell in the sprite sheet */
static
VALUE ray_sprite_sprite_width(VALUE self) {
  say_sprite *sprite = ray_rb2sprite(self);
  if (say_sprite_uses_sprite_sheet(sprite))
    return rb_float_new(say_sprite_get_sprite_width(sprite));
  else
    return Qnil;
}

/* @return [Float, nil] height of a cell in the sprite she e*/
static
VALUE ray_sprite_sprite_height(VALUE self) {
  say_sprite *sprite = ray_rb2sprite(self);
  if (say_sprite_uses_sprite_sheet(sprite))
    return rb_float_new(say_sprite_get_sprite_height(sprite));
  else
    return Qnil;
}

/* Disables usage of sprite sheet */
static
VALUE ray_sprite_disable_sprite_sheet(VALUE self) {
  rb_check_frozen(self);
  say_sprite_disable_sprite_sheet(ray_rb2sprite(self));
  return self;
}

/* @return [true, false] True when using sprite sheets */
static
VALUE ray_sprite_uses_sprite_sheet(VALUE self) {
  return say_sprite_uses_sprite_sheet(ray_rb2sprite(self)) ?
    Qtrue : Qfalse;
}

void Init_ray_sprite() {
  ray_cSprite = rb_define_class_under(ray_mRay, "Sprite", ray_cDrawable);

  rb_define_alloc_func(ray_cSprite, ray_sprite_alloc);

  rb_define_method(ray_cSprite, "image=", ray_sprite_set_image, 1);
  rb_define_method(ray_cSprite, "sub_rect=", ray_sprite_set_sub_rect, 1);
  rb_define_method(ray_cSprite, "color=", ray_sprite_set_color, 1);
  rb_define_method(ray_cSprite, "flip_x=", ray_sprite_set_flip_x, 1);
  rb_define_method(ray_cSprite, "flip_y=", ray_sprite_set_flip_y, 1);

  rb_define_method(ray_cSprite, "image", ray_sprite_image, 0);
  rb_define_method(ray_cSprite, "sub_rect", ray_sprite_sub_rect, 0);
  rb_define_method(ray_cSprite, "color", ray_sprite_color, 0);
  rb_define_method(ray_cSprite, "x_flipped?", ray_sprite_x_flipped, 0);
  rb_define_method(ray_cSprite, "y_flipped?", ray_sprite_y_flipped, 0);

  rb_define_method(ray_cSprite, "sheet_size=", ray_sprite_set_sheet_size, 1);
  rb_define_method(ray_cSprite, "sheet_size", ray_sprite_sheet_size, 0);

  rb_define_method(ray_cSprite, "sheet_pos=", ray_sprite_set_sheet_pos, 1);
  rb_define_method(ray_cSprite, "sheet_pos", ray_sprite_sheet_pos, 0);

  rb_define_method(ray_cSprite, "sprite_height", ray_sprite_sprite_height, 0);
  rb_define_method(ray_cSprite, "sprite_width", ray_sprite_sprite_width, 0);

  rb_define_method(ray_cSprite, "disable_sprite_sheet",
                   ray_sprite_disable_sprite_sheet, 0);
  rb_define_method(ray_cSprite, "uses_sprite_sheet?",
                   ray_sprite_uses_sprite_sheet, 0);
}
