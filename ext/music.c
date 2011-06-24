#include "ray.h"

VALUE ray_cMusic = Qnil;

say_music *ray_rb2music(VALUE obj) {
  if (!RAY_IS_A(obj, rb_path2class("Ray::Music"))) {
    rb_raise(rb_eTypeError, "Can't convert %s into Ray::Music",
             RAY_OBJ_CLASSNAME(obj));
  }

  say_music *music;
  Data_Get_Struct(obj, say_music, music);

  return music;
}

static
VALUE ray_music_alloc(VALUE self) {
  say_music *music = say_music_create();
  return Data_Wrap_Struct(self, NULL, say_music_free, music);
}

/*
  @overload open(filename)
    @param [String] filename Name of the file containing the music to load.
*/
static
VALUE ray_music_open(VALUE self, VALUE arg) {
  say_music *music = ray_rb2music(self);
  if (!say_music_open(music, StringValuePtr(arg))) {
    rb_raise(rb_eRuntimeError, "%s", say_error_get_last());
  }

  return self;
}

/* @return [true, false] True if the music is looping */
static
VALUE ray_music_is_looping(VALUE self) {
  return say_music_is_looping(ray_rb2music(self)) ? Qtrue : Qfalse;
}

/* @overload looping=(val) */
static
VALUE ray_music_set_looping(VALUE self, VALUE val) {
  say_music_set_looping(ray_rb2music(self), RTEST(val));
  return val;
}

/*
  @overload seek(time)
    Seeks to a specific time in the music.
    @param [Float] time Time in seconds
*/
static
VALUE ray_music_seek(VALUE self, VALUE time) {
  say_music_seek(ray_rb2music(self), NUM2DBL(time));
  return time;
}

/* @return [Float] current playing offset, in seconds */
static
VALUE ray_music_time(VALUE self) {
  return rb_float_new(say_music_get_time(ray_rb2music(self)));
}

/* @return [Float] Duration of the music. 0 when no buffer is attached to the
 *   music.
 */
static
VALUE ray_music_duration(VALUE self) {
  return rb_float_new(say_music_get_duration(ray_rb2music(self)));
}

/* Plays the music (or resume from a pause) */
static
VALUE ray_music_play(VALUE self) {
  say_music_play(ray_rb2music(self));
  return self;
}

/* Pauses the music. Can be resumed using play. */
static
VALUE ray_music_pause(VALUE self) {
  say_music_pause(ray_rb2music(self));
  return self;
}

/* Stops the music */
static
VALUE ray_music_stop(VALUE self) {
  say_music_stop(ray_rb2music(self));
  return self;
}

void Init_ray_music() {
  ray_cMusic = rb_define_class_under(ray_mRay, "Music", ray_cAudioSource);
  rb_define_alloc_func(ray_cMusic, ray_music_alloc);

  rb_define_method(ray_cMusic, "open", ray_music_open, 1);

  rb_define_method(ray_cMusic, "looping?", ray_music_is_looping, 0);
  rb_define_method(ray_cMusic, "looping=", ray_music_set_looping, 1);

  rb_define_method(ray_cMusic, "seek", ray_music_seek, 1);
  rb_define_method(ray_cMusic, "time", ray_music_time, 0);

  rb_define_method(ray_cMusic, "duration", ray_music_duration, 0);

  rb_define_method(ray_cMusic, "play", ray_music_play, 0);
  rb_define_method(ray_cMusic, "pause", ray_music_pause, 0);
  rb_define_method(ray_cMusic, "stop", ray_music_stop, 0);
}
