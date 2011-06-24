#include "say.h"

static say_context *say_shared_context = NULL;

static say_thread_variable *say_current_context = NULL;
static say_thread_variable *say_ensured_context = NULL;

static void say_context_create_initial();
static void say_context_setup(say_context *context);
static void say_context_setup_states(say_context *context);

static uint32_t say_context_count = 0;

say_context *say_context_current() {
  if (!say_current_context) {
    say_current_context =
      say_thread_variable_create((say_destructor)say_context_free);
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

  context->dis = window->dis;
  context->win = window->win;

  context->should_free_window = 0;

  GLXContext shared = say_shared_context->context;

  context->context = glXCreateNewContext(window->dis, window->config,
                                         GLX_RGBA_TYPE, shared, True);

  say_context_setup_states(context);

  return context;
}

void say_context_free(say_context *context) {
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

  free(context);
}

void say_context_make_current(say_context *context) {
  if (say_context_current() != context) {
    glXMakeCurrent(context->dis, context->win, context->context);
    say_thread_variable_set(say_current_context, context);
  }
}

void say_context_update(say_context *context) {
  if (context->win)
    glXSwapBuffers(context->dis, context->win);
}

static void say_context_create_initial() {
  say_shared_context = (say_context*)malloc(sizeof(say_context));

  say_context_setup(say_shared_context);
  say_context_setup_states(say_shared_context);

  say_context_make_current(say_shared_context);
  glewInit();

  /* Identify GLSL version to be used */
  const GLubyte *str = glGetString(GL_SHADING_LANGUAGE_VERSION);
  if (str && (str[0] > (GLubyte)'1' || str[2] >= (GLubyte)'4')) { /* 1.40 supported */
    say_shader_enable_new_glsl();
  }
}

static void say_context_setup(say_context *context) {
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
  GLXFBConfig *fbc = glXChooseFBConfig(context->dis, screen, visual_attribs, &fbcount);
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

  GLXContext shared = say_shared_context == context ? NULL : say_shared_context->context;
  context->context = glXCreateNewContext(context->dis, config, GLX_RGBA_TYPE, shared, True);
}

static void say_context_setup_states(say_context *context) {
  say_context_make_current(context);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
}
