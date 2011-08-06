#include "say.h"

#ifdef HAVE_GL_GLXEXT_H
# include <GL/glxext.h>
#else
# define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
# define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092
# define GLX_CONTEXT_PROFILE_MASK_ARB  0x9126

# define GLX_CONTEXT_CORE_PROFILE_BIT_ARB          0x00000001
# define GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#endif

typedef GLXContext (*say_glx_create_context)(Display *dpy,
                                             GLXFBConfig config,
                                             GLXContext share_context,
                                             Bool direct,
                                             const int *attrib_list);

static say_glx_create_context glXCreateContextAttribs = NULL;

say_imp_context say_imp_context_create() {
  return say_imp_context_create_shared(NULL);
}

static GLXContext say_x11_do_create_context(Display     *dis,
                                            GLXFBConfig  conf,
                                            GLXContext   share) {
  glXCreateContextAttribs = (say_glx_create_context)
    glXGetProcAddress((GLubyte*)"glXCreateContextAttribsARB");

  say_context_config *say_conf = say_context_get_config();
  size_t major = say_conf->major_version, minor = say_conf->minor_version;

  if (major >= 3 && glXCreateContextAttribs) {
    GLint attribs[] = {
      GLX_CONTEXT_MAJOR_VERSION_ARB, major,
      GLX_CONTEXT_MINOR_VERSION_ARB, minor,
      GLX_CONTEXT_PROFILE_MASK_ARB, say_conf->core_profile ?
      GLX_CONTEXT_CORE_PROFILE_BIT_ARB :
      GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
      None
    };

    GLXContext ctxt = glXCreateContextAttribs(dis, conf, share, true,
                                              attribs);

    if (ctxt)
      return ctxt;
  }

  return glXCreateNewContext(dis, conf, GLX_RGBA_TYPE, share, True);
}

say_imp_context say_imp_context_create_shared(say_imp_context shared) {
  say_x11_context *context = malloc(sizeof(say_x11_context));

  context->should_free_window = true;

  context->dis = XOpenDisplay(NULL);

  int screen = DefaultScreen(context->dis);

  say_context_config *say_conf = say_context_get_config();
  int visual_attribs[] = {
    GLX_RED_SIZE,     8,
    GLX_GREEN_SIZE,   8,
    GLX_BLUE_SIZE,    8,
    GLX_ALPHA_SIZE,   8,
    GLX_DEPTH_SIZE,   say_conf->depth_size,
    GLX_STENCIL_SIZE, say_conf->stencil_size,
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
  context->context = say_x11_do_create_context(context->dis, config,
                                               glx_shared);

  return context;
}

say_imp_context  say_imp_context_create_for_window(say_imp_context shared,
                                                   say_imp_window  window) {
  say_imp_context context = malloc(sizeof(say_x11_context));

  context->dis = window->dis;
  context->win = window->win;

  context->should_free_window = 0;

  context->context = say_x11_do_create_context(window->dis, window->config,
                                               shared->context);

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
  glXSwapBuffers(context->dis, context->win);
}

