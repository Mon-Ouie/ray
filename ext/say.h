#ifndef SAY_H_
#define SAY_H_

#ifdef _DARWIN_C_SOURCE
# undef _DARWIN_C_SOURCE
#endif

#ifdef HAVE_X11_EXTENSIONS_XRANDR_H
# define HAVE_XRANDR 1
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>

#include <pthread.h>

#include <GL/glew.h>

#ifdef SAY_OSX
# include "say_osx.h"
#else
# include <X11/Xlib.h>
# include <X11/Xatom.h>
# ifdef HAVE_XRANDR
#  include <X11/extensions/Xrandr.h>
# endif
# include <GL/glx.h>
#endif

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_BITMAP_H

#ifdef SAY_OSX
# include <OpenAL/al.h>
# include <OpenAL/alc.h>
#else
# include <AL/al.h>
# include <AL/alc.h>
#endif

#include <sndfile.h>

#ifdef __cplusplus
extern "C" {
# if 0
}
# endif
#endif

#define SAY_PI 3.14159265358979323846

typedef enum {
  SAY_POS_ID = 0,
  SAY_COLOR_ID,
  SAY_TEX_COORD_ID,

  SAY_ATTR_COUNT
} say_attr_id;

#define SAY_POS_ATTR             "in_Vertex"
#define SAY_COLOR_ATTR           "in_Color"
#define SAY_TEX_COORD_ATTR       "in_TexCoord"

#define SAY_PROJECTION_ATTR      "in_Projection"
#define SAY_MODEL_VIEW_ATTR      "in_ModelView"

#define SAY_TEXTURE_ATTR         "in_Texture"
#define SAY_TEXTURE_ENABLED_ATTR "in_TextureEnabled"

#define SAY_FRAG_COLOR           "out_FragColor"

typedef enum {
  SAY_PROJECTION_LOC_ID = 0,
  SAY_MODEL_VIEW_LOC_ID,

  SAY_TEXTURE_LOC_ID,
  SAY_TEXTURE_ENABLED_LOC_ID,

  SAY_LOC_ID_COUNT
} say_attr_loc_id;

#define SAY_STATIC  GL_STATIC_DRAW_ARB
#define SAY_STREAM  GL_STREAM_DRAW_ARB
#define SAY_DYNAMIC GL_DYNAMIC_DRAW_ARB

typedef void  (*say_destructor)(void *data);
typedef void  (*say_creator)(void *data);

typedef struct {
  say_destructor destroy;
  say_destructor create;

  void   *buf;
  size_t  size;
  size_t  capa;

  size_t elem_size;
} say_array;

typedef struct {
  uint32_t key;
  void *value;
} say_table_pair;

typedef struct {
  say_table_pair *pairs;
  say_destructor destructor;
  size_t size;
} say_table;

typedef struct {
  pthread_key_t key;
} say_thread_variable;

typedef struct {
  uint32_t count;

#ifdef SAY_OSX
  SayContext *context;
#else
  GLXContext context;

  Display *dis;
  Window win;

  uint8_t should_free_window;
#endif
} say_context;

typedef struct {
  float content[16];
} say_matrix;

typedef struct {
  float x, y;
} __attribute__((packed)) say_vector2;

typedef struct {
  float x, y, z;
} __attribute__((packed)) say_vector3;

typedef struct {
  float x, y, w, h;
} say_rect;

typedef struct {
  uint8_t r, g, b, a;
} __attribute__((packed)) say_color;

typedef enum {
  SAY_FLOAT,
  SAY_INT,
  SAY_UBYTE,
  SAY_BOOL,

  SAY_COLOR,
  SAY_VECTOR2,
  SAY_VECTOR3
} say_vertex_elem_type;

typedef struct {
  say_vertex_elem_type  type;
  char                 *name;
} say_vertex_elem;

typedef struct {
  say_array *elements;
} say_vertex_type;

/* We don't want compiler to align this structure because we're going to
 * allocate vertices without using sizeof(say_vertex). */
typedef struct {
  say_vector2 pos;
  say_color   col;
  say_vector2 tex;
} __attribute__((packed)) say_vertex;

typedef struct {
  size_t vtype;

  GLuint vbo;
  GLenum type;

  say_table *vaos;

  say_array *buffer;
} say_buffer;

typedef struct {
  size_t buf_id;
  size_t loc;

  size_t vtype;
  size_t size;
} say_buffer_slice;

typedef struct {
  GLuint program;

  GLuint frag_shader;
  GLuint vertex_shader;

  GLint locations[SAY_LOC_ID_COUNT];
} say_shader;

typedef void (*say_fill_proc)(void *data, void *vertices);
typedef void (*say_render_proc)(void *data, size_t first, say_shader *shader);

typedef struct {
  size_t            vertex_count;
  size_t            vtype;
  say_buffer_slice *slice;

  void *data;

  say_fill_proc   fill_proc;
  say_render_proc render_proc;

  say_shader *shader;
  say_matrix *matrix;

  say_vector2 origin;
  say_vector2 scale;
  say_vector2 pos;
  float       z_order;
  float       angle;

  bool use_texture;
  bool matrix_updated;
  bool custom_matrix;
  bool has_changed;
} say_drawable;

typedef struct {
  say_matrix *matrix;
  uint8_t matrix_updated;
  uint8_t custom_matrix;
  uint8_t has_changed;

  say_vector2 size;
  say_vector2 center;

  say_rect viewport;
  uint8_t viewport_changed;

  uint8_t flip_y;
} say_view;

typedef struct {
  say_buffer *buffer;
  say_drawable **drawables;

  size_t size;

  size_t current_vertex;
  size_t current_drawable;
} say_buffer_renderer;

typedef struct {
  say_shader *shader;
  uint8_t using_texture;
} say_renderer;

typedef say_context *(*say_context_proc)(void *data);
typedef void (*say_bind_hook)(void *data);

typedef struct {
  say_thread_variable *context;
  say_context_proc context_proc;
  say_bind_hook bind_hook;

  uint8_t own_context_needed;

  void *data;

  say_renderer *renderer;

  say_view *view;
  say_vector2 size;

  uint8_t up_to_date;
  uint8_t view_up_to_date;
} say_target;

typedef enum {
  SAY_EVENT_NONE = 0,

  SAY_EVENT_QUIT,

  SAY_EVENT_RESIZE,

  SAY_EVENT_MOUSE_ENTERED,
  SAY_EVENT_MOUSE_LEFT,
  SAY_EVENT_FOCUS_GAIN,
  SAY_EVENT_FOCUS_LOSS,

  SAY_EVENT_WHEEL_MOTION,
  SAY_EVENT_MOUSE_MOTION,

  SAY_EVENT_BUTTON_PRESS,
  SAY_EVENT_BUTTON_RELEASE,

  SAY_EVENT_TEXT_ENTERED,

  SAY_EVENT_KEY_PRESS,
  SAY_EVENT_KEY_RELEASE
} say_event_type;

typedef enum {
  SAY_BUTTON_UNKNOWN = 0,

  SAY_BUTTON_LEFT,
  SAY_BUTTON_RIGHT,
  SAY_BUTTON_MIDDLE
} say_button;

typedef enum {
  SAY_KEY_UNKNOWN = 0,

  SAY_KEY_A,
  SAY_KEY_B,
  SAY_KEY_C,
  SAY_KEY_D,
  SAY_KEY_E,
  SAY_KEY_F,
  SAY_KEY_G,
  SAY_KEY_H,
  SAY_KEY_I,
  SAY_KEY_J,
  SAY_KEY_K,
  SAY_KEY_L,
  SAY_KEY_M,
  SAY_KEY_N,
  SAY_KEY_O,
  SAY_KEY_P,
  SAY_KEY_Q,
  SAY_KEY_R,
  SAY_KEY_S,
  SAY_KEY_T,
  SAY_KEY_U,
  SAY_KEY_V,
  SAY_KEY_W,
  SAY_KEY_X,
  SAY_KEY_Y,
  SAY_KEY_Z,

  SAY_KEY_NUM_0,
  SAY_KEY_NUM_1,
  SAY_KEY_NUM_2,
  SAY_KEY_NUM_3,
  SAY_KEY_NUM_4,
  SAY_KEY_NUM_5,
  SAY_KEY_NUM_6,
  SAY_KEY_NUM_7,
  SAY_KEY_NUM_8,
  SAY_KEY_NUM_9,

  SAY_KEY_KP_0,
  SAY_KEY_KP_1,
  SAY_KEY_KP_2,
  SAY_KEY_KP_3,
  SAY_KEY_KP_4,
  SAY_KEY_KP_5,
  SAY_KEY_KP_6,
  SAY_KEY_KP_7,
  SAY_KEY_KP_8,
  SAY_KEY_KP_9,

  SAY_KEY_ESCAPE,
  SAY_KEY_BACKSPACE,
  SAY_KEY_TAB,
  SAY_KEY_RETURN,
  SAY_KEY_PAGE_UP,
  SAY_KEY_PAGE_DOWN,
  SAY_KEY_END,
  SAY_KEY_HOME,
  SAY_KEY_INSERT,
  SAY_KEY_DELETE,
  SAY_KEY_PAUSE,

  SAY_KEY_LCONTROL,
  SAY_KEY_LSHIFT,
  SAY_KEY_LMETA,
  SAY_KEY_LSUPER,

  SAY_KEY_RCONTROL,
  SAY_KEY_RSHIFT,
  SAY_KEY_RMETA,
  SAY_KEY_RSUPER,

  SAY_KEY_MENU,
  SAY_KEY_LBRACKET,
  SAY_KEY_RBRACKET,
  SAY_KEY_SEMICOLON,
  SAY_KEY_COMMA,
  SAY_KEY_PERIOD,
  SAY_KEY_QUOTE,
  SAY_KEY_SLASH,
  SAY_KEY_BACKSLASH,
  SAY_KEY_TILDE,
  SAY_KEY_EQUAL,
  SAY_KEY_MINUS,
  SAY_KEY_PLUS,
  SAY_KEY_ASTERISK,
  SAY_KEY_SPACE,

  SAY_KEY_LEFT,
  SAY_KEY_RIGHT,
  SAY_KEY_UP,
  SAY_KEY_DOWN,

  SAY_KEY_F1,
  SAY_KEY_F2,
  SAY_KEY_F3,
  SAY_KEY_F4,
  SAY_KEY_F5,
  SAY_KEY_F6,
  SAY_KEY_F7,
  SAY_KEY_F8,
  SAY_KEY_F9,
  SAY_KEY_F10,
  SAY_KEY_F11,
  SAY_KEY_F12,
  SAY_KEY_F13,
  SAY_KEY_F14,
  SAY_KEY_F15,

  SAY_KEY_COUNT
} say_key;

#define SAY_MOD_META    0x1
#define SAY_MOD_CONTROL 0x2
#define SAY_MOD_SHIFT   0x4
#define SAY_MOD_SUPER   0x8

typedef struct {
  say_vector2 pos;
} say_event_mouse_motion;

typedef struct {
  say_vector2 pos;
  int delta;
} say_event_wheel_motion;

typedef struct {
  say_vector2 pos;
  say_button button;
} say_event_button;

typedef struct {
  uint32_t text;
} say_event_text;

typedef struct {
  say_key  code;
  uint8_t  mod;
  uint32_t native_code;
} say_event_key;

typedef struct {
  say_vector2 size;
} say_event_resize;

typedef struct {
  say_event_type type;

  union {
    say_event_mouse_motion motion;
    say_event_wheel_motion wheel;
    say_event_button       button;
    say_event_text         text;
    say_event_key          key;
    say_event_resize       resize;
  } ev;
} say_event;

typedef struct {
  uint8_t keys[SAY_KEY_COUNT - 1]; /* Don't count SAY_KEY_UNKNOWN */
  say_vector2 mouse_pos;
} say_input;

typedef struct {
  say_target *target;
  say_input input;

  bool show_cursor;

#ifdef SAY_OSX
  SayWindow *win;
#else
  Display *dis;
  Window win;
  int screen_id;

  XVisualInfo *vi;
  GLXFBConfig config;

  say_event cached_event;

  XIM im;
  XIC ic;

  Cursor hidden_cursor;

  int old_video_mode;

  Atom delete_event;

  uint8_t style;
#endif
} say_window;

typedef struct {
  say_vector2 pos;
  say_color col;
  say_color outline_color;
} say_polygon_point;

typedef struct {
  say_drawable *drawable;

  say_polygon_point *points;
  size_t point_count;

  float outline_width;
  uint8_t outlined;
  uint8_t filled;
} say_polygon;

typedef struct {
  GLuint texture;

  say_color *pixels;
  uint8_t texture_updated;

  size_t width, height;

  uint8_t smooth;
} say_image;

typedef struct {
  GLuint fbo, rbo;
  say_image *img;
  say_target *target;
} say_image_target;

typedef struct {
  say_drawable *drawable;
  say_image *image;

  say_color color;
  say_rect  rect;

  uint8_t flip_x, flip_y;

  bool is_sheet;
  int sheet_w, sheet_h;
  int sheet_x, sheet_y;
} say_sprite;

typedef struct {
  int offset;
  say_rect bounds, sub_rect;
} say_glyph;

typedef struct {
  size_t current_width, height, y;
} say_font_row;

typedef struct {
  say_table *glyphs;
  say_array *rows;

  say_image *image;

  size_t current_height;
} say_font_page;

typedef struct {
  FT_Library library;
  FT_Face face;

  say_table *pages;
} say_font;

#define SAY_TEXT_NORMAL     0x0
#define SAY_TEXT_BOLD       0x1
#define SAY_TEXT_ITALIC     0x2
#define SAY_TEXT_UNDERLINED 0x4

typedef struct {
  say_drawable *drawable;

  say_font *font;
  size_t size;

  uint32_t *string;
  size_t str_length;

  uint8_t style;

  say_color color;

  say_vector2 rect_size;
  uint8_t rect_updated;

  say_vector2 last_img_size;

  size_t underline_vertex;
} say_text;

typedef struct {
  ALuint buf;

  short *samples;
  size_t sample_count;

  float duration;
} say_sound_buffer;

typedef enum {
  SAY_STATUS_STOPPED,
  SAY_STATUS_PAUSED,
  SAY_STATUS_PLAYING
} say_audio_status;

typedef struct {
  ALuint src;
} say_audio_source;

typedef struct {
  say_audio_source *src;
  say_sound_buffer *buf;
} say_sound;

typedef void *(*say_thread_func)(void *data);

typedef struct {
  pthread_t th;
} say_thread;

#define SAY_MUSIC_BUF_COUNT 3

typedef struct {
  say_audio_source *src;
  say_thread *thread;

  ALuint buffers[SAY_MUSIC_BUF_COUNT];
  ALuint last_buffer;

  bool continue_running;
  bool streaming;

  bool looping;

  SF_INFO info;
  SNDFILE *file;

  ALenum format;

  float duration;
  float played_time;
} say_music;

typedef struct {
  void *buf;
  sf_count_t size;
  sf_count_t pos;
} say_vfile;

#define SAY_WINDOW_RESIZABLE  0x1
#define SAY_WINDOW_NO_FRAME   0x2
#define SAY_WINDOW_FULLSCREEN 0x4

/* Initializers */

#define say_make_vector2(x, y)    ((say_vector2){x, y})
#define say_make_vector3(x, y, z) ((say_vector3){x, y, z})
#define say_make_rect(x, y, w, h) ((say_rect){x, y, w, h})
#define say_make_color(r, g, b, a) ((say_color){r, g, b, a})
#define say_make_vertex(pos, col, tex) ((say_vertex){pos, col, tex})
#define say_make_vfile(str, len) ((say_vfile){str, len, 0})

#define say_setup_vio(io)                                  \
  io.get_filelen = (sf_vio_get_filelen)say_vfile_get_size; \
  io.seek        = (sf_vio_seek)say_vfile_seek;            \
  io.read        = (sf_vio_read)say_vfile_read;            \
  io.write       = (sf_vio_write)say_vfile_write;          \
  io.tell        = (sf_vio_tell)say_vfile_tell;

/* Clean up */
void say_clean_up();

/* String manipulations */
uint32_t say_utf8_to_utf32(const uint8_t *string);
char *say_strdup(const char *str);

/* Thread variables */

say_thread_variable *say_thread_variable_create(say_destructor destructor);
void say_thread_variable_free(say_thread_variable *var);

void say_thread_variable_set(say_thread_variable *var, void *val);
void *say_thread_variable_get(say_thread_variable *var);

/* Errors */

const char *say_error_get_last();
void say_error_set(const char *message);

void say_error_clean_up();

/* Context */

void say_context_ensure();

say_context *say_context_current();

say_context *say_context_create_for_window(say_window *window);
say_context *say_context_create();
void say_context_free(say_context *context);

void say_context_make_current(say_context *context);
void say_context_update(say_context *context);

void say_context_clean_up();

/* Vertex types */

size_t say_vertex_type_make_new();

say_vertex_type *say_get_vertex_type(size_t i);

void say_vertex_type_push(say_vertex_type *type, say_vertex_elem elem);

say_vertex_elem_type say_vertex_type_get_type(say_vertex_type *type, size_t i);
const char *say_vertex_type_get_name(say_vertex_type *type, size_t i);
size_t say_vertex_type_get_elem_count(say_vertex_type *type);
size_t say_vertex_type_get_size(say_vertex_type *type);
size_t say_vertex_type_get_offset(say_vertex_type *type, size_t elem);

void say_vertex_type_clean_up();

/* Views */

say_view *say_view_create();
void say_view_free(say_view *view);
void say_view_copy(say_view *view, say_view *other);

void say_view_zoom_by(say_view *view, say_vector2 scale);

void say_view_set_size(say_view *view, say_vector2 size);
void say_view_set_center(say_view *view, say_vector2 center);
void say_view_set_viewport(say_view *view, say_rect viewport);

say_vector2 say_view_get_size(say_view *view);
say_vector2 say_view_get_center(say_view *view);
say_rect say_view_get_viewport(say_view *view);

void say_view_flip_y(say_view *view, uint8_t val);
uint8_t say_view_is_y_flipped(say_view *view);

say_matrix *say_view_get_matrix(say_view *view);
void say_view_set_matrix(say_view *view, say_matrix *matrix);

uint8_t say_view_has_changed(say_view *view);
void say_view_apply(say_view *view, say_shader *shader, say_vector2 size);

/* Targets */

say_target *say_target_create();
void say_target_free(say_target *target);

void say_target_set_context_proc(say_target *target, say_context_proc proc);
void say_target_need_own_contxt(say_target *target, uint8_t val);
say_context *say_target_get_context(say_target *target);

void say_target_set_bind_hook(say_target *target, say_bind_hook proc);

void say_target_set_custom_data(say_target *target, void *data);

void say_target_set_size(say_target *target, say_vector2 size);
say_vector2 say_target_get_size(say_target *target);

void say_target_set_view(say_target *target, say_view *view);
say_view *say_target_get_view(say_target *target);
say_view *say_target_get_default_view(say_target *target);

say_shader *say_target_get_shader(say_target *target);

say_rect say_target_get_clip(say_target *target);
say_rect say_target_get_viewport_for(say_target *target, say_rect rect);

int say_target_make_current(say_target *target);

void say_target_clear(say_target *target, say_color color);
void say_target_draw(say_target *target, say_drawable *drawable);
void say_target_draw_buffer(say_target *target,
                            say_buffer_renderer *buf);
void say_target_update(say_target *target);

/* Inputs */

uint8_t say_input_is_holding(say_input *input, say_key key);
say_vector2 say_input_get_mouse_pos(say_input *input);

void say_input_reset(say_input *input);

void say_input_press(say_input *input, say_key key);
void say_input_release(say_input *input, say_key key);

void say_input_set_mouse_pos(say_input *input, say_vector2 pos);

/* Windows */

say_window *say_window_create();
void say_window_free(say_window *window);

int say_window_open(say_window *win, size_t w, size_t h, const char *title,
                    uint8_t style);
void say_window_close(say_window *win);

void say_window_update(say_window *win);

void say_window_hide_cursor(say_window *win);
void say_window_show_cursor(say_window *win);

bool say_window_set_icon(say_window *win, say_image *icon);

int  say_window_poll_event(say_window *win, say_event *ev);
void say_window_wait_event(say_window *win, say_event *ev);

say_input *say_window_get_input(say_window *win);

/* Matrices */

say_matrix *say_matrix_identity();
void say_matrix_free(say_matrix *matrix);

say_matrix *say_matrix_translation(float x, float y, float z);
say_matrix *say_matrix_scale(float x, float y, float z);
say_matrix *say_matrix_rotation(float angle, float x, float y, float z);
say_matrix *say_matrix_ortho(float left, float right, float bottom, float top,
                             float near, float far);
say_matrix *say_matrix_perspective(float fovy, float aspect,
                                   float near, float far);
say_matrix *say_matrix_looking_at(float eye_x, float eye_y, float eye_z,
                                  float center_x, float center_y,
                                  float center_z,
                                  float up_x, float up_y, float up_z);

void say_matrix_set(say_matrix *matrix, int x, int y, float value);
float say_matrix_get(say_matrix *matrix, int x, int y);

say_vector3 say_matrix_transform(say_matrix *matrix, say_vector3 init);

float say_matrix_cofactor(say_matrix *matrix, int x, int y);
say_matrix *say_matrix_comatrix(say_matrix *matrix);
say_matrix *say_matrix_inverse(say_matrix *matrix);

void say_matrix_set_content(say_matrix *matrix, float *content);
float *say_matrix_get_content(say_matrix *matrix);

void say_matrix_reset(say_matrix *matrix);

void say_matrix_multiply_by(say_matrix *matrix, say_matrix *other);
void say_matrix_translate_by(say_matrix *matrix, float x, float y, float z);
void say_matrix_scale_by(say_matrix *matrix, float x, float y, float z);
void say_matrix_rotate(say_matrix *matrix, float angle, float x, float y,
                       float z);
void say_matrix_set_ortho(say_matrix *matrix,
                          float left, float right, float bottom, float top,
                          float near, float far);
void say_matrix_set_perspective(say_matrix *matrix,
                                float fovy, float aspect,
                                float near, float far);
void say_matrix_look_at(say_matrix *matrix,
                        float eye_x, float eye_y, float eye_z,
                        float center_x, float center_y, float center_z,
                        float up_x, float up_y, float up_z);

/* VBOs */

say_buffer *say_buffer_create(size_t vtype, GLenum type, size_t size);
void say_buffer_free(say_buffer *buf);

void *say_buffer_get_vertex(say_buffer *buf, size_t id);

void say_buffer_bind(say_buffer *buf);
void say_buffer_unbind();

void say_buffer_update_part(say_buffer *buf, size_t index, size_t size);
void say_buffer_update(say_buffer *buf);

size_t say_buffer_get_size(say_buffer *buf);
void say_buffer_resize(say_buffer *buf, size_t size);

/* Buffer slices */

say_buffer_slice *say_buffer_slice_create(size_t vtype, size_t size);
void say_buffer_slice_free(say_buffer_slice *slice);

void say_buffer_slice_recreate(say_buffer_slice *slice, size_t size);

size_t say_buffer_slice_get_loc(say_buffer_slice *slice);
size_t say_buffer_slice_get_size(say_buffer_slice *slice);

void *say_buffer_slice_get_vertex(say_buffer_slice *slice, size_t id);

void say_buffer_slice_update(say_buffer_slice *slice);
void say_buffer_slice_bind(say_buffer_slice *slice);

void say_buffer_slice_clean_up();

/* Shaders */

say_shader *say_shader_create();
void say_shader_free(say_shader *shader);

void say_shader_enable_new_glsl();
void say_shader_force_old();

int say_shader_compile_frag(say_shader *shader, const char *src);
int say_shader_compile_vertex(say_shader *shader, const char *src);

void say_shader_apply_vertex_type(say_shader *shader, size_t vtype);

int say_shader_link(say_shader *shader);

void say_shader_set_matrix(say_shader *shader, const char *name,
                           say_matrix *matrix);
void say_shader_set_current_texture(say_shader *shader, const char *name);
void say_shader_set_int(say_shader *shader, const char *name, int val);

void say_shader_set_matrix_id(say_shader *shader, say_attr_loc_id id,
                              say_matrix *matrix);
void say_shader_set_current_texture_id(say_shader *shader, say_attr_loc_id id);
void say_shader_set_int_id(say_shader *shader, say_attr_loc_id id, int val);

int say_shader_locate(say_shader *shader, const char *name);

void say_shader_set_vector2_loc(say_shader *shader, int loc, say_vector2 val);
void say_shader_set_vector3_loc(say_shader *shader, int loc, say_vector3 val);
void say_shader_set_color_loc(say_shader *shader, int loc, say_color val);
void say_shader_set_matrix_loc(say_shader *shader, int loc, say_matrix *val);
void say_shader_set_float_loc(say_shader *shader, int loc, float val);
void say_shader_set_floats_loc(say_shader *shader, int loc, size_t count, float *val);
void say_shader_set_image_loc(say_shader *shader, int loc, say_image *val);
void say_shader_set_current_texture_loc(say_shader *shader, int loc);
void say_shader_set_bool_loc(say_shader *shader, int loc, uint8_t val);

void say_shader_bind(say_shader *shader);

/* Drawables */

say_drawable *say_drawable_create(size_t vtype);
void say_drawable_free(say_drawable *drawable);

void say_drawable_copy(say_drawable *drawable, say_drawable *other);

void say_drawable_set_custom_data(say_drawable *drawable, void *data);

void say_drawable_set_vertex_count(say_drawable *drawable, size_t size);
size_t say_drawable_get_vertex_count(say_drawable *drawable);

size_t say_drawable_get_vertex_type(say_drawable *drawable);

void say_drawable_set_fill_proc(say_drawable *drawable, say_fill_proc proc);
void say_drawable_set_render_proc(say_drawable *drawable, say_render_proc proc);

void say_drawable_fill_buffer(say_drawable *drawable, void *vertices);
void say_drawable_fill_own_buffer(say_drawable *drawable);
void say_drawable_draw_at(say_drawable *drawable, size_t id, say_shader *shader);
void say_drawable_draw(say_drawable *drawable, say_shader *shader);

void say_drawable_set_changed(say_drawable *drawable);
uint8_t say_drawable_has_changed(say_drawable *drawable);

void say_drawable_set_textured(say_drawable *drawable, uint8_t val);
uint8_t say_drawable_is_textured(say_drawable *drawable);

say_shader *say_drawable_get_shader(say_drawable *drawable);
void say_drawable_set_shader(say_drawable *drawable, say_shader *shader);

void say_drawable_set_origin(say_drawable *drawable, say_vector2 origin);
void say_drawable_set_scale(say_drawable *drawable, say_vector2 scale);
void say_drawable_set_pos(say_drawable *drawable, say_vector2 pos);
void say_drawable_set_z(say_drawable *drawable, float z);
void say_drawable_set_angle(say_drawable *drawable, float angle);

say_vector2 say_drawable_get_origin(say_drawable *drawable);
say_vector2 say_drawable_get_scale(say_drawable *drawable);
say_vector2 say_drawable_get_pos(say_drawable *drawable);
float say_drawable_get_z(say_drawable *drawable);
float say_drawable_get_angle(say_drawable *drawable);

say_matrix *say_drawable_get_matrix(say_drawable *drawable);
void say_drawable_set_matrix(say_drawable *drawable, say_matrix *matrix);
say_vector3 say_drawable_transform(say_drawable *drawable, say_vector3 point);

/* Renderers */

say_renderer *say_renderer_create();
void say_renderer_free(say_renderer *renderer);

say_shader *say_renderer_get_shader(say_renderer *renderer);

void say_renderer_reset_states(say_renderer *renderer);
void say_renderer_push(say_renderer *renderer, say_drawable *drawable);
void say_renderer_push_buffer(say_renderer *renderer,
                              say_buffer_renderer *buf);

/* Buffer renderers */

say_buffer_renderer *say_buffer_renderer_create(GLenum type,
                                                size_t vertex_count,
                                                size_t size);
void say_buffer_renderer_free(say_buffer_renderer *renderer);

void say_buffer_renderer_clear(say_buffer_renderer *renderer);
void say_buffer_renderer_push(say_buffer_renderer *renderer,
                              say_drawable *drawable);
void say_buffer_renderer_update(say_buffer_renderer *renderer);

void say_buffer_renderer_render(say_buffer_renderer *renderer,
                                say_shader *shader);

/* Polygons */

say_polygon *say_polygon_create(size_t size);
void say_polygon_free(say_polygon *polygon);

void say_polygon_copy(say_polygon *polygon, say_polygon *other);

say_polygon *say_polygon_triangle(say_vector2 a, say_vector2 b, say_vector2 c,
                                  say_color color);
say_polygon *say_polygon_rectangle(say_vector2 pos, say_vector2 size,
                                     say_color color);
say_polygon *say_polygon_circle(say_vector2 center, float radius, say_color color);
say_polygon *say_polygon_ellipse(say_vector2 center, float rx, float ry,
                                 say_color color);
say_polygon *say_polygon_line(say_vector2 first, say_vector2 last, float width,
                              say_color color);

void say_polygon_resize(say_polygon *polygon, size_t resize);

void say_polygon_set_color(say_polygon *polygon, say_color color);
void say_polygon_set_outline_color(say_polygon *polygon, say_color color);

size_t say_polygon_get_size(say_polygon *polygon);

void say_polygon_set_pos_for(say_polygon *polygon, size_t id, say_vector2 pos);
void say_polygon_set_color_for(say_polygon *polygon, size_t id, say_color col);
void say_polygon_set_outline_for(say_polygon *polygon, size_t id, say_color col);

say_vector2 say_polygon_get_pos_for(say_polygon *polygon, size_t id);
say_color say_polygon_get_color_for(say_polygon *polygon, size_t id);
say_color say_polygon_get_outline_for(say_polygon *polygon, size_t id);

void say_polygon_set_outline(say_polygon *polygon, float size);
float say_polygon_get_outline(say_polygon *polygon);

uint8_t say_polygon_outlined(say_polygon *polygon);
uint8_t say_polygon_filled(say_polygon *polygon);

void say_polygon_set_outlined(say_polygon *polygon, uint8_t val);
void say_polygon_set_filled(say_polygon *polygon, uint8_t val);

/* Images */

say_image *say_image_create();
void say_image_free(say_image *img);

size_t say_image_get_width(say_image *img);
size_t say_image_get_height(say_image *img);
say_vector2 say_image_get_size(say_image *img);

void say_image_resize(say_image *img, size_t w, size_t h);

void say_image_load_raw(say_image *img, size_t width, size_t height,
                        say_color *pixels);
int say_image_load_file(say_image *img, const char *filename);
int say_image_load_from_memory(say_image *img, size_t size, const char *buffer);
void say_image_create_with_size(say_image *img, size_t w, size_t h);

uint8_t say_image_is_smooth(say_image *img);
void say_image_set_smooth(say_image *img, uint8_t val);

say_color say_image_get(say_image *img, size_t x, size_t y);
void say_image_set(say_image *img, size_t x, size_t y, say_color color);

say_rect say_image_get_tex_rect(say_image *img, say_rect rect);

say_color *say_image_get_buffer(say_image *img);

void say_image_bind(say_image *img);
void say_image_unbind();

void say_image_update_texture(say_image *img);

/* Image targets */

say_image_target *say_image_target_create();
void say_image_target_free(say_image_target *target);

void say_image_target_set_image(say_image_target *target, say_image *image);
say_image *say_image_target_get_image(say_image_target *target);
void say_image_target_update(say_image_target *taget);

void say_image_target_bind(say_image_target *target);
void say_image_target_unbind();

/* Sprites */

say_sprite *say_sprite_create();
void say_sprite_free(say_sprite *sprite);

say_image *say_sprite_get_image(say_sprite *sprite);
void say_sprite_set_image(say_sprite *sprite, say_image *img);

say_color say_sprite_get_color(say_sprite *sprite);
void say_sprite_set_color(say_sprite *sprite, say_color color);

say_rect say_sprite_get_rect(say_sprite *sprite);
void say_sprite_set_rect(say_sprite *sprite, say_rect rect);

void say_sprite_flip_x(say_sprite *sprite, uint8_t flip_x);
void say_sprite_flip_y(say_sprite *sprite, uint8_t flip_y);

uint8_t say_sprite_is_x_flipped(say_sprite *sprite);
uint8_t say_sprite_is_y_flipped(say_sprite *sprite);

bool say_sprite_uses_sprite_sheet(say_sprite *sprite);
void say_sprite_disable_sprite_sheet(say_sprite *sprite);

void say_sprite_set_sheet_size(say_sprite *sprite, say_vector2 size);
void say_sprite_set_sheet_pos(say_sprite *sprite, say_vector2 pos);

say_vector2 say_sprite_get_sheet_pos(say_sprite *sprite);
say_vector2 say_sprite_get_sheet_size(say_sprite *sprite);

float say_sprite_get_sprite_width(say_sprite *sprite);
float say_sprite_get_sprite_height(say_sprite *sprite);

/* Arrays */

say_array *say_array_create(size_t size_el, say_destructor dtor,
                            say_creator ctor);
void say_array_free(say_array *ary);

void   *say_array_get(say_array *ary, size_t i);
void    say_array_next(say_array *ary, void **i);
void   *say_array_get_end(say_array *ary);
size_t  say_array_get_size(say_array *ary);
size_t  say_array_get_elem_size(say_array *ary);

void say_array_resize(say_array *ary, size_t size);
void say_array_insert(say_array *ary, size_t i, void *elem);
void say_array_push(say_array *ary, void *elem);
void say_array_delete(say_array *ary, size_t i);

/* Tables */

say_table *say_table_create(say_destructor destructor);
void say_table_free(say_table *table);

void *say_table_get(say_table *table, uint32_t id);
void  say_table_set(say_table *table, uint32_t id, void *value);
void  say_table_del(say_table *table, uint32_t id);

/* Fonts */

say_font *say_font_create();
void say_font_free(say_font *font);

say_font *say_font_default();

int say_font_load_from_file(say_font *font, const char *file);
int say_font_load_from_memory(say_font *font, void *buf, size_t size);

say_glyph *say_font_get_glyph(say_font *font, uint32_t codepoint, size_t size, uint8_t bold);
size_t say_font_get_kerning(say_font *font, uint32_t first, uint32_t second,
                            size_t size);
size_t say_font_get_line_height(say_font *font, size_t size);
say_image *say_font_get_image(say_font *font, size_t size);

void say_font_clean_up();

/* Texts */

say_text *say_text_create();
void say_text_free(say_text *text);

uint32_t *say_text_get_string(say_text *text);
size_t say_text_get_string_length(say_text *text);
void say_text_set_string(say_text *text, uint32_t *string, size_t length);

say_font *say_text_get_font(say_text *text);
void say_text_set_font(say_text *text, say_font *font);

size_t say_text_get_size(say_text *text);
void say_text_set_size(say_text *text, size_t size);

uint8_t say_text_get_style(say_text *text);
void say_text_set_style(say_text *text, size_t style);

say_color say_text_get_color(say_text *text);
void say_text_set_color(say_text *text, say_color col);

say_rect say_text_get_rect(say_text *text);

/* Audio context */

void say_audio_context_ensure();
void say_audio_context_clean_up();

/* Audio */

void say_audio_set_volume(float vol);
void say_audio_set_pos(say_vector3 pos);
void say_audio_set_direction(say_vector3 dir);

float say_audio_get_volume();
say_vector3 say_audio_get_pos();
say_vector3 say_audio_get_direction();

ALenum say_audio_get_format(size_t channel_count);

/* Sound buffers */

say_sound_buffer *say_sound_buffer_create();
void say_sound_buffer_free(say_sound_buffer *buf);

int say_sound_buffer_load_from_memory(say_sound_buffer *buf, size_t len,
                                       const char *str);
int say_sound_buffer_load_from_file(say_sound_buffer *buf, const char *filename);

short *say_sound_buffer_get_samples(say_sound_buffer *buf);
size_t say_sound_buffer_get_sample_count(say_sound_buffer *buf);
size_t say_sound_buffer_get_channel_count(say_sound_buffer *buf);
size_t say_sound_buffer_get_sample_rate(say_sound_buffer *buf);
float say_sound_buffer_get_duration(say_sound_buffer *buf);

/* Audio sources */

say_audio_source *say_audio_source_create();
void say_audio_source_free(say_audio_source *src);

void say_audio_source_set_pitch(say_audio_source *src, float pitch);
float say_audio_source_get_pitch(say_audio_source *src);

void say_audio_source_set_volume(say_audio_source *src, float vol);
float say_audio_source_get_volume(say_audio_source *src);

void say_audio_source_set_pos(say_audio_source *src, say_vector3 pos);
say_vector3 say_audio_source_get_pos(say_audio_source *src);

void say_audio_source_set_relative(say_audio_source *src, uint8_t rel);
uint8_t say_audio_source_get_relative(say_audio_source *src);

void say_audio_source_set_min_distance(say_audio_source *src, float dist);
float say_audio_source_get_min_distance(say_audio_source *src);

void say_audio_source_set_attenuation(say_audio_source *src, float att);
float say_audio_source_get_attenuation(say_audio_source *src);

say_audio_status say_audio_source_get_status(say_audio_source *src);

/* Sounds */

say_sound *say_sound_create();
void say_sound_free(say_sound *snd);

void say_sound_set_buffer(say_sound *snd, say_sound_buffer *buf);
say_sound_buffer *say_sound_get_buffer(say_sound *snd);

void say_sound_set_looping(say_sound *snd, uint8_t val);
uint8_t say_sound_is_looping(say_sound *snd);

void say_sound_seek(say_sound *snd, float time);
float say_sound_get_time(say_sound *snd);

float say_sound_get_duration(say_sound *snd);

void say_sound_play(say_sound *snd);
void say_sound_pause(say_sound *snd);
void say_sound_stop(say_sound *snd);

/* Virtual files */

sf_count_t say_vfile_get_size(say_vfile *file);
sf_count_t say_vfile_seek(sf_count_t offset, int whence, say_vfile *file);
sf_count_t say_vfile_read(void *buf, sf_count_t count, say_vfile *file);
sf_count_t say_vfile_write(const void *buf, sf_count_t count, say_vfile *file);
sf_count_t say_vfile_tell(say_vfile *file);

/* Threads */

say_thread *say_thread_create(void *data, say_thread_func func);
void say_thread_free(say_thread *th);

void say_thread_join(say_thread *th);

/* Musics */

say_music *say_music_create();
void say_music_free(say_music *music);

bool say_music_open(say_music *music, const char *filename);

void say_music_set_looping(say_music *music, bool val);
bool say_music_is_looping(say_music *music);

void say_music_seek(say_music *music, float time);
float say_music_get_time(say_music *music);

float say_music_get_duration(say_music *music);

void say_music_play(say_music *music);
void say_music_pause(say_music *music);
void say_music_stop(say_music *music);

#ifdef __cplusplus
# if 0
{
# endif
}
#endif

#endif
