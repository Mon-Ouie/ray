#include "say.h"

#ifdef SAY_OSX
# include "say_osx_context.h"
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

#ifdef SAY_OSX
  SayContext *shared = say_shared_context->context;
  context->context = [[SayContext alloc] initWithShared:shared];
  [context->context setView:[window->win view]];
#else
  context->dis = window->dis;
  context->win = window->win;

  context->should_free_window = 0;

  GLXContext shared = say_shared_context->context;

  context->context = glXCreateNewContext(window->dis, window->config,
                                         GLX_RGBA_TYPE, shared, True);
#endif

  say_context_setup_states(context);

  return context;
}

void say_context_free(say_context *context) {
#ifdef SAY_OSX
  [context->context release];
#else
  if (context->context) {
    if (glXGetCurrentContext() == context->context)
      glXMakeCurrent(context->dis, None, NULL);
  }

  if (say_context_current() == context) {
    say_thread_variable_set(say_current_context, NULL);
  }

  if (context->should_free_window) {
    if (context->win) {
      XDestroyWindow(context->dis, context->win);
      XFlush(context->dis);
    }

    XCloseDisplay(context->dis);
  }
#endif

  free(context);
}

void say_context_make_current(say_context *context) {
  if (say_context_current() != context) {
#ifdef SAY_OSX
    [context->context makeCurrent];
#else
    glXMakeCurrent(context->dis, context->win, context->context);
#endif

    say_thread_variable_set(say_current_context, context);
  }
}

void say_context_update(say_context *context) {
#ifdef SAY_OSX
  [context->context update];
#else
  if (context->win)
    glXSwapBuffers(context->dis, context->win);
#endif
}

static void say_context_create_initial() {
  say_shared_context = (say_context*)malloc(sizeof(say_context));

  say_context_setup(say_shared_context);
  say_context_setup_states(say_shared_context);

  say_context_make_current(say_shared_context);
  say_context_glew_init();

  /* Identify GLSL version to be used */
  const GLubyte *str = glGetString(GL_SHADING_LANGUAGE_VERSION);

  /* if GLSL 1.40 is supported */
  if (str && (str[0] > (GLubyte)'1' || str[2] >= (GLubyte)'4')) {
    say_shader_enable_new_glsl();
  }
}

static void say_context_setup(say_context *context) {
#ifdef SAY_OSX
  SayContext *shared = say_shared_context == context ? nil :
    say_shared_context->context;

  context->context = [[SayContext alloc] initWithShared:shared];
#else
  context->dis = XOpenDisplay(NULL);

  int screen = DefaultScreen(context->dis);

  static int visual_attribs[] = {
    GLX_RED_SIZE,     8,
    GLX_GREEN_SIZE,   8,
    GLX_BLUE_SIZE,    8,
    GLX_ALPHA_SIZE,   8,
    GLX_DEPTH_SIZE,   24,
    GLX_DOUBLEBUFFER, True,
    None
  };

  int fbcount;
  GLXFBConfig *fbc = glXChooseFBConfig(context->dis, screen, visual_attribs,
                                       &fbcount);
  GLXFBConfig config = fbc[0];
  XFree(fbc);

  XVisualInfo *vi = glXGetVisualFromFBConfig(context->dis, config);

  XSetWindowAttributes swa;
  Colormap cmap;
  swa.colormap = cmap = XCreateColormap(context->dis,
                                        RootWindow(context->dis, screen),
                                        vi->visual, AllocNone);
  swa.background_pixmap = None ;
  swa.border_pixel      = 0;

  context->win = XCreateWindow(context->dis, RootWindow(context->dis, screen),
                               0, 0, 1, 1,
                               0,
                               vi->depth,
                               InputOutput,
                               vi->visual,
                               CWBorderPixel|CWColormap, &swa);

  XFree(vi);

  GLXContext shared = say_shared_context == context ? NULL :
    say_shared_context->context;
  context->context = glXCreateNewContext(context->dis, config, GLX_RGBA_TYPE,
                                         shared, True);
#endif
}

static void say_context_setup_states(say_context *context) {
  say_context_make_current(context);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
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
