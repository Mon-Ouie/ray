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

static void say_context_create_initial();
static void say_context_setup(say_context *context);
static void say_context_setup_states(say_context *context);
static void say_context_glew_init();

static uint32_t say_context_count = 0;

say_context *say_context_current() {
  if (!say_current_context) {
    say_current_context = say_thread_variable_create(NULL);
  }

  return (say_context*)say_thread_variable_get(say_current_context);
}

void say_context_ensure() {
  if (!say_ensured_context) {
    say_ensured_context =
      say_thread_variable_create((say_destructor)say_context_free);
  }

  if (!say_context_current()) {
    say_context *context = say_thread_variable_get(say_ensured_context);

    if (!context) {
      context = say_context_create();
      say_thread_variable_set(say_ensured_context, context);
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
  say_context_setup_states(say_shared_context);

  say_context_make_current(say_shared_context);
  say_context_glew_init();

  /* Identify GLSL version to be used */
  const GLubyte *str = glGetString(GL_SHADING_LANGUAGE_VERSION);

  /* if GLSL 1.30 and GL_EXT_gpu_shader4 are supported */
  if (__GLEW_EXT_gpu_shader4) {
    if (str && (str[0] > (GLubyte)'1' || str[2] >= (GLubyte)'3')) {
      say_shader_enable_new_glsl();
    }
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
  if (say_current_context)
    say_thread_variable_free(say_current_context);

  if (say_ensured_context)
    say_thread_variable_free(say_ensured_context);

  say_current_context = NULL;
  say_ensured_context = NULL;
}

static void say_context_glew_init() {
  glewInit();

  if (__GLEW_APPLE_vertex_array_object &&
      !__GLEW_ARB_vertex_array_object) {
    glBindVertexArray    = glBindVertexArrayAPPLE;
    glGenVertexArrays    = (PFNGLGENVERTEXARRAYSPROC)glGenVertexArraysAPPLE;
    glDeleteVertexArrays = glDeleteVertexArraysAPPLE;
  }
}
