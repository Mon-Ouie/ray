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

  /* Perform GLEW's job. */
  glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)
    say_get_proc("glBindVertexArray");
  glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)
    say_get_proc("glDeleteVertexArrays");
  glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)
    say_get_proc("glGenVertexArrays");

  glBindFragDataLocation = (PFNGLBINDFRAGDATALOCATIONPROC)
    say_get_proc("glBindFragDataLocation");

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

  glVertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISORPROC)
    say_get_proc("glVertexAttribDivisor");

  /* Vertex attribs */
  replace(glVertexAttribPointerARB, glVertexAttribPointer);
  replace(glEnableVertexAttribArrayARB, glEnableVertexAttribArray);
  replace(glVertexAttribDivisorARB, glVertexAttribDivisor);
  replace(glGetVertexAttribivARB, glGetVertexAttribiv);
  replace(glDisableVertexAttribArrayARB, glDisableVertexAttribArray);

  /* Perform GLEW's job. */
  glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)
    say_get_proc("glGenFramebuffers");
  glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)
    say_get_proc("glDeleteFramebuffers");
  glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)
    say_get_proc("glBindFramebuffer");
  glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)
    say_get_proc("glFramebufferTexture2D");
  glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)
    say_get_proc("glFramebufferRenderbuffer");

  /* Framebuffer objects */
  replace(glGenFramebuffersEXT, glGenFramebuffers);
  replace(glDeleteFramebuffersEXT, glDeleteFramebuffers);
  replace(glBindFramebufferEXT, glBindFramebuffer);
  replace(glFramebufferTexture2DEXT, glFramebufferTexture2D);
  replace(glFramebufferRenderbufferEXT, glFramebufferRenderbuffer);

  glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)
    say_get_proc("glGenRenderbuffers");
  glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)
    say_get_proc("glDeleteRenderbuffers");
  glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)
    say_get_proc("glBindRenderbuffer");
  glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)
    say_get_proc("glRenderbufferStorage");

  /* Renderbuffer objects */
  replace(glGenRenderbuffersEXT, glGenRenderbuffers);
  replace(glDeleteRenderbuffersEXT, glDeleteRenderbuffers);
  replace(glBindRenderbufferEXT, glBindRenderbuffer);
  replace(glRenderbufferStorageEXT, glRenderbufferStorage);

  glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)
    say_get_proc("glGenerateMipmap");

  /* Mipmaps */
  replace(glGenerateMipmapEXT, glGenerateMipmap);

  /*
   * Debug output
   */
  glDebugMessageCallbackARB = (PFNGLDEBUGMESSAGECALLBACKARBPROC)
    say_get_proc("glDebugMessageCallbackARB");

#undef replace
}
