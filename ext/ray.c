#include "ray.h"

VALUE ray_mRay = Qnil;

#ifdef SAY_OSX
static NSAutoreleasePool *ray_osx_pool = nil;
#endif

static
VALUE ray_clean_up(VALUE self) {
  say_clean_up();
  return Qnil;
}

/* @return [Ray::Vector2] Size of the screen */
static
VALUE ray_screen_size(VALUE self) {
  return ray_vector2_to_rb(say_make_vector2(say_imp_screen_get_width(),
                                            say_imp_screen_get_height()));
}

void Init_ray_ext() {
#ifdef SAY_OSX
  say_osx_flip_pool();
#endif

  ray_mRay = rb_define_module("Ray");

  rb_define_private_method(rb_singleton_class(ray_mRay),
                           "_clean_up!", ray_clean_up, 0);

  rb_define_module_function(ray_mRay, "screen_size", ray_screen_size, 0);

  Init_ray_vector();
  Init_ray_rect();
  Init_ray_matrix();
  Init_ray_color();
  Init_ray_vertex();
  Init_ray_gl();
  Init_ray_gl_vertex();
  Init_ray_int_array();
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
