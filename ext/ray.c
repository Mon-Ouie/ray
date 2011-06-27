#include "ray.h"

VALUE ray_mRay = Qnil;

void Init_ray_ext() {
  ray_mRay = rb_define_module("Ray");

  Init_ray_vector();
  Init_ray_rect();
  Init_ray_matrix();
  Init_ray_color();
  Init_ray_vertex();
  Init_ray_image();
  Init_ray_font();
  Init_ray_shader();
  Init_ray_view();
  Init_ray_drawable();
  Init_ray_polygon();
  Init_ray_sprite();
  Init_ray_text();
  Init_ray_target();
  Init_ray_window();
  Init_ray_image_target();
  Init_ray_input();
  Init_ray_event();
  Init_ray_audio();
  Init_ray_sound_buffer();
  Init_ray_audio_source();
  Init_ray_sound();
  Init_ray_music();
}
