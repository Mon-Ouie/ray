#include "say.h"

say_imp_context say_imp_context_create() {
  return  say_imp_context_create_shared(NULL);
}

say_imp_context say_imp_context_create_shared(say_imp_context shared) {
  say_x11_context *context = malloc(sizeof(say_x11_context));

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

  GLXContext glx_shared = shared ? shared->context : NULL;
  context->context = glXCreateNewContext(context->dis, config, GLX_RGBA_TYPE,
                                         glx_shared, True);

  return context;
}

say_imp_context  say_imp_context_create_for_window(say_imp_context shared,
                                                   say_imp_window  window) {
  say_imp_context context = malloc(sizeof(say_x11_context));

  context->dis = window->dis;
  context->win = window->win;

  context->should_free_window = 0;

  context->context = glXCreateNewContext(window->dis, window->config,
                                         GLX_RGBA_TYPE, shared->context, True);

  return context;
}

void say_imp_context_free(say_imp_context context) {
  if (context->context) {
    if (glXGetCurrentContext() == context->context)
      glXMakeCurrent(context->dis, None, NULL);
  }

  if (context->should_free_window) {
    if (context->win) {
      XDestroyWindow(context->dis, context->win);
      XFlush(context->dis);
    }

    XCloseDisplay(context->dis);
  }
}

void say_imp_context_make_current(say_imp_context context) {
  glXMakeCurrent(context->dis, context->win, context->context);
}

void say_imp_context_update(say_imp_context context) {
  if (context->win)
    glXSwapBuffers(context->dis, context->win);
}

