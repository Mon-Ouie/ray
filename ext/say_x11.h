#include <stdbool.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#ifdef HAVE_XRANDR
# include <X11/extensions/Xrandr.h>
#endif

#include <GL/glx.h>

#include "mo.h"

typedef struct say_x11_window {
  Display *dis;
  Window win;
  int screen_id;

  XVisualInfo *vi;
  GLXFBConfig config;

  mo_array events;

  XIM im;
  XIC ic;

  Cursor hidden_cursor;

  int old_video_mode;

  Atom delete_event;

  bool fullscreen;
} say_x11_window;

typedef struct say_x11_context {
  GLXContext context;

  Display *dis;
  Window   win;

  bool should_free_window;
} say_x11_context;
