#include "ray.h"

VALUE ray_mAudio = Qnil;

/*
  @return [Float] The global volume. 0 is the minimum volume, whereas 100
  is the maximum.
*/
VALUE ray_audio_volume(VALUE self) {
  return rb_float_new(say_audio_get_volume());
}

/*
  @overload volume=(val)
  Sets the global volume.
  @param [Float] val The new value for the volume.
*/
VALUE ray_audio_set_volume(VALUE self, VALUE value) {
  float volume = (float)NUM2DBL(value);
  say_audio_set_volume(volume);

  return value;
}

/*
  @return [Ray::Vector3] The position of the listener.
*/
VALUE ray_audio_pos(VALUE self) {
  return ray_vector3_to_rb(say_audio_get_pos());
}

/*
  @overload pos=(pos)
  Sets the position of the listener.
  @param [Ray::Vector3, #to_vector3] pos The new position of the
  listener.
*/
VALUE ray_audio_set_pos(VALUE self, VALUE pos) {
  say_audio_set_pos(ray_convert_to_vector3(pos));
  return pos;
}

/*
  @return [Ray::Vector3] The orientation of the listener.
*/
VALUE ray_audio_direction(VALUE self) {
  return ray_vector3_to_rb(say_audio_get_direction());
}

/*
  @overload direction=(dir)
  Sets the orientation of the listener.
  @param [Ray::Vector3, #to_vector3] dir The new orientation of the listener.
*/
VALUE ray_audio_set_direction(VALUE self, VALUE dir) {
  say_audio_set_direction(ray_convert_to_vector3(dir));
  return dir;
}

void Init_ray_audio() {
  ray_mAudio = rb_define_module_under(ray_mRay, "Audio");
  rb_define_module_function(ray_mAudio, "volume", ray_audio_volume, 0);
  rb_define_module_function(ray_mAudio, "volume=", ray_audio_set_volume, 1);
  rb_define_module_function(ray_mAudio, "pos", ray_audio_pos, 0);
  rb_define_module_function(ray_mAudio, "pos=", ray_audio_set_pos, 1);
  rb_define_module_function(ray_mAudio, "direction", ray_audio_direction, 0);
  rb_define_module_function(ray_mAudio, "direction=", ray_audio_set_direction, 1);
}
