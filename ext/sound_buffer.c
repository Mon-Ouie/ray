#include "ray.h"

VALUE ray_cSoundBuffer = Qnil;

say_sound_buffer *ray_rb2sound_buffer(VALUE obj) {
  if (!RAY_IS_A(obj, rb_path2class("Ray::SoundBuffer"))) {
    rb_raise(rb_eTypeError, "Can't convert %s into Ray::SoundBuffer",
             RAY_OBJ_CLASSNAME(obj));
  }

  say_sound_buffer *buf;
  Data_Get_Struct(obj, say_sound_buffer, buf);

  return buf;
}

static
VALUE ray_sound_buffer_alloc(VALUE self) {
  say_sound_buffer *buf = say_sound_buffer_create();
  return Data_Wrap_Struct(self, NULL, say_sound_buffer_free, buf);
}

/*
  @overload initialize(io_or_string)
    Loads the sound buffer out of an IO object or a string.
 */
static
VALUE ray_sound_buffer_init(VALUE self, VALUE arg) {
  say_sound_buffer *buf = ray_rb2sound_buffer(self);

  if (rb_respond_to(arg, RAY_METH("read"))) {
    arg = rb_funcall(arg, RAY_METH("read"), 0);

    if (!say_sound_buffer_load_from_memory(buf, RSTRING_LEN(arg),
                                           StringValuePtr(arg))) {
      rb_raise(rb_eRangeError, "%s", say_error_get_last());
    }
  }
  else {
    if (!say_sound_buffer_load_from_file(buf, StringValuePtr(arg))) {
      rb_raise(rb_eRangeError, "%s", say_error_get_last());
    }
  }

  return self;
}

/* @return [Float] duration of the sound in seconds */
static
VALUE ray_sound_buffer_duration(VALUE self) {
  return rb_float_new(say_sound_buffer_get_duration(ray_rb2sound_buffer(self)));
}

/* @return [Integer] amount of channels in the sound */
static
VALUE ray_sound_buffer_channel_count(VALUE self) {
  return INT2FIX(say_sound_buffer_get_channel_count(ray_rb2sound_buffer(self)));
}

/* @return [Integer] sample rate */
static
VALUE ray_sound_buffer_sample_rate(VALUE self) {
  return INT2FIX(say_sound_buffer_get_sample_rate(ray_rb2sound_buffer(self)));
}

void Init_ray_sound_buffer() {
  ray_cSoundBuffer = rb_define_class_under(ray_mRay, "SoundBuffer", rb_cObject);

  rb_define_alloc_func(ray_cSoundBuffer, ray_sound_buffer_alloc);
  rb_define_method(ray_cSoundBuffer, "initialize", ray_sound_buffer_init, 1);

  rb_define_method(ray_cSoundBuffer, "duration", ray_sound_buffer_duration, 0);
  rb_define_method(ray_cSoundBuffer, "channel_count", ray_sound_buffer_channel_count, 0);
  rb_define_method(ray_cSoundBuffer, "sample_rate", ray_sound_buffer_sample_rate, 0);
}
