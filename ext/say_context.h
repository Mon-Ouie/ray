#ifndef SAY_CONTEXT_H_
#define SAY_CONTEXT_H_

#include "say_basic_type.h"

typedef struct {
  size_t depth_size;
  size_t stencil_size;

  size_t major_version, minor_version;

  bool core_profile;
  bool debug;
} say_context_config;

struct say_window;

typedef struct {
  uint32_t count;
  say_imp_context context;

  GLuint texture;

  GLuint  vao;
  GLuint  vbo;
  void   *buffer_obj;

  GLuint program;

  GLuint ibo;

  GLuint pack_pbo;
  GLuint unpack_pbo;

  void *target;

  bool blend_enabled;
  GLenum src_blend_func;
  GLenum dst_blend_func;
} say_context;

say_context_config *say_context_get_config();

void say_context_free_el(void *context);

void say_context_ensure();

say_context *say_context_current();
mo_array    *say_context_get_all();

say_context *say_context_create_for_window(struct say_window *window);
say_context *say_context_create();
void say_context_free(say_context *context);

void say_context_make_current(say_context *context);
void say_context_update(say_context *context);

void say_context_clean_up();

#endif
