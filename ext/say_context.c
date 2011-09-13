#include "say.h"

#ifdef SAY_OSX
# include "say_osx_context.h"
#endif

#ifdef SAY_X11
# include "say_x11_context.h"
#endif

#ifdef SAY_WIN
# include "say_win_context.h"
#endif

static say_context *say_shared_context = NULL;

static say_thread_variable *say_current_context = NULL;
static say_thread_variable *say_ensured_context = NULL;

static mo_array *say_all_ensured_contexts = NULL;

static void say_context_create_initial();
static void say_context_setup(say_context *context);
static void say_context_setup_states(say_context *context);
static void say_context_setup_cache(say_context *context);
static void say_context_glew_init();

static uint32_t say_context_count = 0;

say_context_config *say_context_get_config() {
  static say_context_config conf = {
    24, 0, /* 24 bit depth buffer, no stencil buffer */
    2, 1,  /* Anything older than 3.x doesn't matter */
    false, /* Let user call deprecated features */
    false  /* Disable debugging */
  };

  return &conf;
}

void say_context_free_el(void *context) {
  say_context_free(*(say_context**)context);
}

say_context *say_context_current() {
  if (!say_current_context) {
    say_current_context = say_thread_variable_create();
  }

  return (say_context*)say_thread_variable_get(say_current_context);
}

mo_array *say_context_get_all() {
  if (!say_all_ensured_contexts) {
    say_all_ensured_contexts = mo_array_create(sizeof(say_context*));
    say_all_ensured_contexts->release = say_context_free_el;
  }

  return say_all_ensured_contexts;
}

void say_context_ensure() {
  if (!say_ensured_context)
    say_ensured_context = say_thread_variable_create();

  if (!say_all_ensured_contexts) {
    say_all_ensured_contexts = mo_array_create(sizeof(say_context*));
    say_all_ensured_contexts->release = say_context_free_el;
  }

  if (!say_context_current()) {
    say_context *context = say_thread_variable_get(say_ensured_context);

    if (!context) {
      context = say_context_create();
      say_thread_variable_set(say_ensured_context, context);
      mo_array_push(say_all_ensured_contexts, &context);
    }

    say_context_make_current(context);
  }
}

say_context *say_context_create() {
  say_context *context = (say_context*)malloc(sizeof(say_context));
  context->count = ++say_context_count;

  if (!say_shared_context)
    say_context_create_initial();

  say_context_setup(context);
  say_context_setup_states(context);
  say_context_setup_cache(context);

  return context;
}

say_context *say_context_create_for_window(say_window *window) {
  say_context *context = (say_context*)malloc(sizeof(say_context));
  context->count = ++say_context_count;

  if (!say_shared_context)
    say_context_create_initial();

  say_imp_context shared = say_shared_context->context;
  context->context = say_imp_context_create_for_window(shared,
                                                       window->win);
  say_context_setup_states(context);
  say_context_setup_cache(context);

  return context;
}

void say_context_free(say_context *context) {
  if (say_context_current() == context) {
    say_thread_variable_set(say_current_context, NULL);
  }

  say_imp_context_free(context->context);

  free(context);
}

void say_context_make_current(say_context *context) {
  if (say_context_current() != context) {
    say_imp_context_make_current(context->context);
    say_thread_variable_set(say_current_context, context);
  }
}

void say_context_update(say_context *context) {
  say_imp_context_update(context->context);
}

static void say_context_create_initial() {
  say_shared_context = (say_context*)malloc(sizeof(say_context));

  say_context_setup(say_shared_context);
  say_context_make_current(say_shared_context);
  say_context_glew_init();

  /* Identify GLSL version to be used */
  const GLubyte *str = glGetString(GL_SHADING_LANGUAGE_VERSION);
  if (str && (str[0] > (GLubyte)'1' || str[2] >= (GLubyte)'3') &&
      glBindFragDataLocation) {
    say_shader_enable_new_glsl();
  }
}

static void say_context_setup(say_context *context) {
  if (say_shared_context == context)
    context->context = say_imp_context_create();
  else {
    context->context =
      say_imp_context_create_shared(say_shared_context->context);
  }
}

static void say_context_setup_states(say_context *context) {
  say_context_make_current(context);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  glReadBuffer(GL_FRONT);
}

static void say_context_setup_cache(say_context *context) {
  context->texture_unit = 0;
  for (size_t i = 0; i < 32; i++)
    context->textures[i] = 0;

  context->vao        = 0;
  context->vbo        = 0;
  context->buffer_obj = NULL;

  context->program = 0;

  context->ibo = 0;

  context->pack_pbo   = 0;
  context->unpack_pbo = 0;

  context->target = NULL;

  context->blend_enabled = false;
  context->src_blend_func = GL_SRC_ALPHA;
  context->dst_blend_func = GL_ONE_MINUS_SRC_ALPHA;

  context->fbo = 0;
  context->rbo = 0;
}

void say_context_clean_up() {
  if (say_all_ensured_contexts)
    mo_array_free(say_all_ensured_contexts);

  if (say_ensured_context)
    say_thread_variable_free(say_ensured_context);

  if (say_current_context)
    say_thread_variable_free(say_current_context);

  say_current_context      = NULL;
  say_ensured_context      = NULL;
  say_all_ensured_contexts = NULL;
}

static void say_context_glew_init() {
  /*
   * Fetch any proc we can.
   */
  glewExperimental = true;
  glewInit();

  /**
   * Load needed extensions.
   *
   * We need to replace pointers for code to work both using OpenGL core
   * features and extensions.
   */

#define replace(old, new)                       \
  if (!new && old)                              \
    new = old;

  /* Shaders */
  replace(glCreateShaderObjectARB, glCreateShader);
  replace(glShaderSourceARB, glShaderSource);
  replace(glCompileShaderARB, glCompileShader);
  replace(glGetObjectParameterivARB, glGetShaderiv);
  replace(glGetObjectParameterivARB, glGetProgramiv);
  replace(glGetInfoLogARB, glGetShaderInfoLog);
  replace(glGetInfoLogARB, glGetProgramInfoLog);
  replace(glGetUniformLocationARB, glGetUniformLocation);
  replace(glCreateProgramObjectARB, glCreateProgram)
  replace(glAttachObjectARB, glAttachShader);
  replace(glDetachObjectARB, glDetachShader);
  replace(glBindAttribLocationARB, glBindAttribLocation);
  replace(glBindFragDataLocationEXT, glBindFragDataLocation);
  replace(glLinkProgramARB, glLinkProgram);
  replace(glUseProgramObjectARB, glUseProgram);
  replace(glDeleteObjectARB, glDeleteShader);
  replace(glDeleteObjectARB, glDeleteProgram);
  replace(glUniform1iARB, glUniform1i);
  replace(glUniform1fARB, glUniform1f);
  replace(glUniform2fARB, glUniform2f);
  replace(glUniform3fARB, glUniform3f);
  replace(glUniform1fvARB, glUniform1fv);
  replace(glUniform2fvARB, glUniform2fv);
  replace(glUniform3fvARB, glUniform3fv);
  replace(glUniform4fvARB, glUniform4fv);
  replace(glUniformMatrix4fvARB, glUniformMatrix4fv);

  /* Buffers */
  replace(glGenBuffersARB, glGenBuffers);
  replace(glDeleteBuffersARB, glDeleteBuffersARB);
  replace(glBindBufferARB, glBindBuffer);
  replace(glBufferDataARB, glBufferData);
  replace(glBufferSubDataARB, glBufferSubData);
  replace(glGetBufferSubDataARB, glGetBufferSubData);

  /* Vertex attribs */
  replace(glVertexAttribPointerARB, glVertexAttribPointer);
  replace(glEnableVertexAttribArrayARB, glEnableVertexAttribArray);
  replace(glVertexAttribDivisorARB, glVertexAttribDivisor);
  replace(glGetVertexAttribivARB, glGetVertexAttribiv);
  replace(glDisableVertexAttribArrayARB, glDisableVertexAttribArray);

  /* Framebuffer objects */
  replace(glGenFramebuffersEXT, glGenFramebuffers);
  replace(glDeleteFramebuffersEXT, glDeleteFramebuffers);
  replace(glBindFramebufferEXT, glBindFramebuffer);
  replace(glFramebufferTexture2DEXT, glFramebufferTexture2D);
  replace(glFramebufferRenderbufferEXT, glFramebufferRenderbuffer);

  /* Renderbuffer objects */
  replace(glGenRenderbuffersEXT, glGenRenderbuffers);
  replace(glDeleteRenderbuffersEXT, glDeleteRenderbuffers);
  replace(glBindRenderbufferEXT, glBindRenderbuffer);
  replace(glRenderbufferStorageEXT, glRenderbufferStorage);

  /* Mipmaps */
  replace(glGenerateMipmapEXT, glGenerateMipmap);
}
