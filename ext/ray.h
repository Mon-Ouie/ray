#ifndef RAY_H_
#define RAY_H_

#include "ruby.h"
#include "say.h"

#ifdef __cplusplus
extern "C" {
# if 0
}
# endif
#endif

typedef struct {
  say_drawable *drawable;
  VALUE         obj;

  size_t vsize;
} ray_drawable;

/* Classes and modules */
extern VALUE ray_mRay;

extern VALUE ray_cVector2;
extern VALUE ray_cVector3;
extern VALUE ray_cRect;
extern VALUE ray_cMatrix;
extern VALUE ray_cColor;
extern VALUE ray_cVertex;
extern VALUE ray_mGL;
extern VALUE ray_cGLVertex;
extern VALUE ray_cIntArray;
extern VALUE ray_cImage;
extern VALUE ray_cFont;
extern VALUE ray_cShader;
extern VALUE ray_cView;
extern VALUE ray_cDrawable;
extern VALUE ray_cPolygon;
extern VALUE ray_cSprite;
extern VALUE ray_cText;
extern VALUE ray_cTarget;
extern VALUE ray_cWindow;
extern VALUE ray_cImageTarget;
extern VALUE ray_cInput;
extern VALUE ray_cEvent;
extern VALUE ray_mAudio;
extern VALUE ray_cSoundBuffer;
extern VALUE ray_cAudioSource;
extern VALUE ray_cSound;
extern VALUE ray_cMusic;

/* Macros for Ruby's C API */

#define RAY_IS_A(obj, klass) (RTEST(rb_obj_is_kind_of(obj, klass)))

#define RAY_OBJ_CLASSNAME(obj) (rb_class2name(rb_class_of(obj)))

#define RAY_SYM(string) (ID2SYM(rb_intern(string)))
#define RAY_METH(string) (rb_intern(string))

#define RAY_ARRAY_AT(ary, i) (rb_funcall(ary, RAY_METH("[]"), 1, INT2FIX(i)))

void Init_ray_ext();
void Init_ray_vector();
void Init_ray_rect();
void Init_ray_matrix();
void Init_ray_color();
void Init_ray_vertex();
void Init_ray_gl();
void Init_ray_gl_vertex();
void Init_ray_int_array();
void Init_ray_image();
void Init_ray_font();
void Init_ray_shader();
void Init_ray_view();
void Init_ray_drawable();
void Init_ray_polygon();
void Init_ray_sprite();
void Init_ray_text();
void Init_ray_target();
void Init_ray_window();
void Init_ray_image_target();
void Init_ray_input();
void Init_ray_event();
void Init_ray_audio();
void Init_ray_sound_buffer();
void Init_ray_audio_source();
void Init_ray_sound();
void Init_ray_music();

/* Conversions */

say_vector2 *ray_rb2vector2_ptr(VALUE obj);
say_vector2 ray_rb2vector2(VALUE obj);
say_vector2 ray_convert_to_vector2(VALUE obj);
VALUE ray_vector2_to_rb(say_vector2 obj);

say_vector3 *ray_rb2vector3_ptr(VALUE obj);
say_vector3 ray_rb2vector3(VALUE obj);
say_vector3 ray_convert_to_vector3(VALUE obj);
VALUE ray_vector3_to_rb(say_vector3 obj);

say_rect *ray_rb2rect_ptr(VALUE obj);
say_rect ray_rb2rect(VALUE obj);
say_rect ray_convert_to_rect(VALUE obj);
VALUE ray_rect2rb(say_rect rect);

VALUE ray_matrix2rb(say_matrix *matrix);
say_matrix *ray_rb2matrix(VALUE matrix);

VALUE ray_view2rb(say_view *view);
say_view *ray_rb2view(VALUE self);

say_color ray_rb2col(VALUE object);
VALUE ray_col2rb(say_color color);

say_vertex *ray_rb2vertex(VALUE obj);

say_image *ray_rb2image(VALUE obj);
say_font *ray_rb2font(VALUE obj);

VALUE ray_shader2rb(say_shader *shader, VALUE owner);
say_shader *ray_rb2shader(VALUE obj);

say_drawable *ray_rb2drawable(VALUE obj);
say_polygon *ray_rb2polygon(VALUE obj);
say_sprite *ray_rb2sprite(VALUE obj);
say_text *ray_rb2text(VALUE obj);

say_target *ray_rb2target(VALUE obj);
say_window *ray_rb2window(VALUE obj);
say_image_target *ray_rb2image_target(VALUE obj);

say_event *ray_rb2event(VALUE obj);

say_input *ray_rb2input(VALUE obj);
VALUE ray_input2rb(say_input *input, VALUE owner);

say_sound_buffer *ray_rb2sound_buffer(VALUE obj);

say_audio_source *ray_rb2audio_source(VALUE obj);
say_sound *ray_rb2sound(VALUE obj);
say_music *ray_rb2music(VALUE obj);

say_array *ray_rb2int_array(VALUE obj);

uint8_t ray_byte_clamp(int color);

VALUE ray_get_vertex_class(size_t id);
size_t ray_get_vtype(VALUE class);

#ifdef __cplusplus
# if 0
{
# endif
}
#endif

#endif
