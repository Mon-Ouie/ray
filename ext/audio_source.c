#include "ray.h"

VALUE ray_cAudioSource = Qnil;

say_audio_source *ray_rb2audio_source(VALUE obj) {
  if (RAY_IS_A(obj, rb_path2class("Ray::Sound")))
    return ray_rb2sound(obj)->src;
  if (RAY_IS_A(obj, rb_path2class("Ray::Music")))
    return ray_rb2music(obj)->src;
  else {
    rb_raise(rb_eTypeError, "can't get audio source pointer from %s",
             RAY_OBJ_CLASSNAME(obj));
  }

  return NULL;
}

/* @see volume= */
static
VALUE ray_audio_source_volume(VALUE self) {
  say_audio_source *source = ray_rb2audio_source(self);
  return rb_float_new(say_audio_source_get_volume(source));
}

/*
 *  @overload volume=(vol)
 *    Sets the volume of the source
 *
 *    The volume is a number between 0 and 100. When it is set to 100, the
 *    volume is maximal.
 *
 *    @param [Float] vol The new volume of the source
 */
static
VALUE ray_audio_source_set_volume(VALUE self, VALUE value) {
  rb_check_frozen(self);

  say_audio_source *source = ray_rb2audio_source(self);
  say_audio_source_set_volume(source, NUM2DBL(value));

  return value;
}

/* @see ptich= */
static
VALUE ray_audio_source_pitch(VALUE self) {
  say_audio_source *source = ray_rb2audio_source(self);
  return rb_float_new(say_audio_source_get_pitch(source));
}

/*
 * @overload pitch=(pitch)
 *   Sets the pitch of the sound
 *
 *   Pitch makes the sound more acute or grave, and affcets playing speed. It is
 *   defaulted to 1.
 *
 *   @param [Float] pitch The new pitch value
 */
static
VALUE ray_audio_source_set_pitch(VALUE self, VALUE pitch) {
  rb_check_frozen(self);

  say_audio_source *source = ray_rb2audio_source(self);
  say_audio_source_set_pitch(source, NUM2DBL(pitch));

  return pitch;
}

/* @see pos= */
static
VALUE ray_audio_source_pos(VALUE self) {
  say_audio_source *source = ray_rb2audio_source(self);
  return ray_vector3_to_rb(say_audio_source_get_pos(source));
}

/*
 * @overload pos=(pos)
 *   Sets the position of the sound
 *   @param [Ray::Vector3] pos The new position of the source
 */
static
VALUE ray_audio_source_set_pos(VALUE self, VALUE pos) {
  rb_check_frozen(self);

  say_audio_source *source = ray_rb2audio_source(self);
  say_audio_source_set_pos(source, ray_convert_to_vector3(pos));

  return pos;
}

/*
 * @see relative=
 */
static
VALUE ray_audio_source_is_relative(VALUE self) {
  say_audio_source *source = ray_rb2audio_source(self);
  return say_audio_source_get_relative(source) ? Qtrue : Qfalse;
}

/*
 *@overload relative=(rel)
 *  Sets whether the sound position is relative to the listener's
 *
 *  This is false by default, meaning the position is absolute.
 *
 *  @param [true, false] rel True to make the sound become relative
 */
static
VALUE ray_audio_source_set_relative(VALUE self, VALUE val) {
  rb_check_frozen(self);

  say_audio_source *source = ray_rb2audio_source(self);
  say_audio_source_set_relative(source, RTEST(val));

  return val;
}

/* @see min_distance= */
static
VALUE ray_audio_source_min_distance(VALUE self) {
  say_audio_source *source = ray_rb2audio_source(self);
  return rb_float_new(say_audio_source_get_min_distance(source));
}

/*
 * @overload min_distance=(dist)
 *   Sets the sounds minimal distance
 *
 *   The minimal distance is the distance beyound which a source's volume will
 *   start decreasing. It is defaulted to one.
 *
 *   @param [Float] dist The new default value
 */
static
VALUE ray_audio_source_set_min_distance(VALUE self, VALUE dist) {
  rb_check_frozen(self);

  say_audio_source *source = ray_rb2audio_source(self);
  say_audio_source_set_min_distance(source, NUM2DBL(dist));

  return dist;
}

/* @see attenuation= */
static
VALUE ray_audio_source_attenuation(VALUE self) {
  say_audio_source *source = ray_rb2audio_source(self);
  return rb_float_new(say_audio_source_get_attenuation(source));
}

/*
 *  @overload attenuation=(attenuation)
 *    Sets the attenuation of a sound
 *
 *    When set to 0, the sound will not be attenuated at all. 100 makes
 *    attenuation very quick.
 *
 *    @param [Float] atenuation New attenuation factor of the sound
 */
static
VALUE ray_audio_source_set_attenuation(VALUE self, VALUE att) {
  rb_check_frozen(self);
  say_audio_source_set_attenuation(ray_rb2audio_source(self), NUM2DBL(att));
  return att;
}

/*
 * Playing status
 *
 * The status can be +:playing+, +:paused+, or +:stopped+.
 *
 * @return [Symbol] Playing status
 */
static
VALUE ray_audio_source_status(VALUE self) {
  switch (say_audio_source_get_status(ray_rb2audio_source(self))) {
  case SAY_STATUS_PAUSED:  return RAY_SYM("paused");
  case SAY_STATUS_PLAYING: return RAY_SYM("playing");
  case SAY_STATUS_STOPPED: return RAY_SYM("stopped");
  }

  return Qnil; /* should never happen */
}

/*
 * Document-class: Ray::AudioSource
 *
 * Audio sources are objects able to produce sound, either by directly passing
 * them to a buffer or by streaming audio data.
 *
 * This class holds generic methods to manipulate a source.
 */
void Init_ray_audio_source() {
  ray_cAudioSource = rb_define_class_under(ray_mRay, "AudioSource", rb_cObject);

  rb_define_method(ray_cAudioSource, "volume", ray_audio_source_volume, 0);
  rb_define_method(ray_cAudioSource, "volume=", ray_audio_source_set_volume, 1);

  rb_define_method(ray_cAudioSource, "pitch", ray_audio_source_pitch, 0);
  rb_define_method(ray_cAudioSource, "pitch=", ray_audio_source_set_pitch, 1);

  /* @group Spatialization */
  rb_define_method(ray_cAudioSource, "pos", ray_audio_source_pos, 0);
  rb_define_method(ray_cAudioSource, "pos=",
                   ray_audio_source_set_pos, 1);

  rb_define_method(ray_cAudioSource, "relative?", ray_audio_source_is_relative,
                   0);
  rb_define_method(ray_cAudioSource, "relative=", ray_audio_source_set_relative,
                   1);

  rb_define_method(ray_cAudioSource, "min_distance",
                   ray_audio_source_min_distance, 0);
  rb_define_method(ray_cAudioSource, "min_distance=",
                   ray_audio_source_set_min_distance, 1);

  rb_define_method(ray_cAudioSource, "attenuation",
                   ray_audio_source_attenuation, 0);
  rb_define_method(ray_cAudioSource, "attenuation=",
                   ray_audio_source_set_attenuation, 1);
  /* @endgroup */

  rb_define_method(ray_cAudioSource, "status", ray_audio_source_status, 0);
}
