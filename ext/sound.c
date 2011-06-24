#include "ray.h"

VALUE ray_cSound = Qnil;

say_sound *ray_rb2sound(VALUE obj) {
  if (!RAY_IS_A(obj, rb_path2class("Ray::Sound"))) {
    rb_raise(rb_eTypeError, "Can't convert %s into Ray::Sound",
             RAY_OBJ_CLASSNAME(obj));
  }

  say_sound *snd;
  Data_Get_Struct(obj, say_sound, snd);

  return snd;
}

static
VALUE ray_sound_alloc(VALUE self) {
  say_sound *snd = say_sound_create();
  return Data_Wrap_Struct(self, NULL, say_sound_free, snd);
}

/*
  @overload buffer=(buf)
    Sets a new buffer for the sound. Causes it to stop playing.
    @param [Ray::SoundBuffer, nil] buf New buffer used by the sound.
 */
static
VALUE ray_sound_set_buffer(VALUE self, VALUE buf) {
  say_sound_set_buffer(ray_rb2sound(self), ray_rb2sound_buffer(buf));
  rb_iv_set(self, "@buffer", buf);
  return buf;
}

/* @return [Ray::SoundBuffer, nil] */
static
VALUE ray_sound_buffer(VALUE self) {
  return rb_iv_get(self, "@buffer");
}

/* @return [true, false] True if the sound is looping */
static
VALUE ray_sound_is_looping(VALUE self) {
  return say_sound_is_looping(ray_rb2sound(self)) ? Qtrue : Qfalse;
}

/* @overload looping=(val) */
static
VALUE ray_sound_set_looping(VALUE self, VALUE val) {
  say_sound_set_looping(ray_rb2sound(self), RTEST(val));
  return val;
}

/*
  @overload seek(time)
    Seeks to a specific time in the sound.
    @param [Float] time Time in seconds
*/
static
VALUE ray_sound_seek(VALUE self, VALUE time) {
  say_sound_seek(ray_rb2sound(self), NUM2DBL(time));
  return time;
}

/* @return [Float] current playing offset, in seconds */
static
VALUE ray_sound_time(VALUE self) {
  return rb_float_new(say_sound_get_time(ray_rb2sound(self)));
}

/* @return [Float] Duration of the sound. 0 when no buffer is attached to the
 *   sound.
 */
static
VALUE ray_sound_duration(VALUE self) {
  return rb_float_new(say_sound_get_duration(ray_rb2sound(self)));
}

/* Plays the sound (or resume from a pause) */
static
VALUE ray_sound_play(VALUE self) {
  say_sound_play(ray_rb2sound(self));
  return self;
}

/* Pauses the sound. Can be resumed using play. */
static
VALUE ray_sound_pause(VALUE self) {
  say_sound_pause(ray_rb2sound(self));
  return self;
}

/* Stops the sound */
static
VALUE ray_sound_stop(VALUE self) {
  say_sound_stop(ray_rb2sound(self));
  return self;
}

void Init_ray_sound() {
  ray_cSound = rb_define_class_under(ray_mRay, "Sound", ray_cAudioSource);
  rb_define_alloc_func(ray_cSound, ray_sound_alloc);

  rb_define_method(ray_cSound, "buffer=", ray_sound_set_buffer, 1);
  rb_define_method(ray_cSound, "buffer", ray_sound_buffer, 0);

  rb_define_method(ray_cSound, "looping?", ray_sound_is_looping, 0);
  rb_define_method(ray_cSound, "looping=", ray_sound_set_looping, 1);

  rb_define_method(ray_cSound, "seek", ray_sound_seek, 1);
  rb_define_method(ray_cSound, "time", ray_sound_time, 0);

  rb_define_method(ray_cSound, "duration", ray_sound_duration, 0);

  rb_define_method(ray_cSound, "play", ray_sound_play, 0);
  rb_define_method(ray_cSound, "pause", ray_sound_pause, 0);
  rb_define_method(ray_cSound, "stop", ray_sound_stop, 0);
}
