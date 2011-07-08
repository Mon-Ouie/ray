#include "say.h"

static void say_window_unbind_fbo(void *window) {
  say_image_target_unbind();
}

#ifdef SAY_OSX
# include "say_osx_window.h"
#endif

#ifdef SAY_X11
unsigned long  say_event_mask =
  FocusChangeMask   | ButtonPressMask        | ButtonReleaseMask |
  ButtonMotionMask  | PointerMotionMask      | KeyPressMask      |
  KeyReleaseMask    | StructureNotifyMask    | EnterWindowMask   |
  LeaveWindowMask;
#endif

say_window *say_window_create() {
  say_window *win = (say_window*)malloc(sizeof(say_window));

  win->target = say_target_create();
  say_input_reset(&win->input);

  win->show_cursor = true;

#ifdef SAY_OSX
  win->win = say_imp_window_create();
#else
  win->dis = NULL;
  win->win = None;

  win->vi = NULL;

  win->im = NULL;
  win->ic = NULL;

  win->hidden_cursor = None;
  win->old_video_mode = -1;

  win->cached_event.type = SAY_EVENT_NONE;
#endif

  return win;
}

#ifdef SAY_X11
static void say_window_find_config(say_window *win) {
  static int visual_attribs[] = {
    GLX_DOUBLEBUFFER, GL_TRUE,
    GLX_RED_SIZE,   8,
    GLX_GREEN_SIZE, 8,
    GLX_BLUE_SIZE,  8,
    GLX_ALPHA_SIZE, 8,
    GLX_DEPTH_SIZE, 24,
    None
  };

  int conf_count;
  GLXFBConfig *configs = glXChooseFBConfig(win->dis, DefaultScreen(win->dis),
                                           visual_attribs, &conf_count);

  win->config = configs[0];
  win->vi = glXGetVisualFromFBConfig(win->dis, configs[0]);
  XFree(configs);
}
#endif

void say_window_free(say_window *win) {
  say_window_close(win);
  say_target_free(win->target);

#ifdef SAY_OSX
  say_imp_window_free(win->win);
#endif

  free(win);
}

#ifdef SAY_X11
static void say_window_build_cursor(say_window *win) {
  Pixmap pixmap = XCreatePixmap(win->dis, win->win, 1, 1, 1);
  GC gc = XCreateGC(win->dis, pixmap, 0, NULL);
  XDrawPoint(win->dis, pixmap, gc, 0, 0);
  XFreeGC(win->dis, gc);

  XColor color;
  color.flags = DoRed | DoGreen | DoBlue;
  color.red = color.blue = color.green = 0;
  win->hidden_cursor = XCreatePixmapCursor(win->dis, pixmap, pixmap,
                                           &color, &color, 0, 0);

  XFreePixmap(win->dis, pixmap);
}
#endif

#ifdef SAY_X11
# ifdef HAVE_XRANDR
static bool say_window_enable_fullscreen(say_window *win, size_t w, size_t h) {
  int version;
  if (!XQueryExtension(win->dis, "RANDR", &version, &version, &version)) {
    say_error_set("can't create fullscreen windows (XRANDR not supported)");
    return false;
  }

  XRRScreenConfiguration *conf = XRRGetScreenInfo(win->dis,
                                                  RootWindow(win->dis,
                                                             win->screen_id));

  if (!conf) {
    say_error_set("failed to get the screen configuration");
    return false;
  }

  Rotation current_rot;
  win->old_video_mode = XRRConfigCurrentConfiguration(conf, &current_rot);

  int size_count = 0;
  XRRScreenSize *sizes = XRRConfigSizes(conf, &size_count);

  if (!sizes || size_count == 0) {
    say_error_set("could not get screen sizes");
    XRRFreeScreenConfigInfo(conf);
    return false;
  }

  for (int i = 0; i < size_count; i++) {
    if (sizes[i].width == (int)w && sizes[i].height == (int)h) {
      XRRSetScreenConfig(win->dis, conf, RootWindow(win->dis, win->screen_id),
                         i, current_rot, CurrentTime);
      XRRFreeScreenConfigInfo(conf);
      return true;
    }
  }

  XRRFreeScreenConfigInfo(conf);
  return false;
}
# else
static bool say_window_enable_fullscreen(say_window *win, size_t w, size_t h) {
  say_error_set("can't create fullscreen windows (XRANDR not supported)");
  return false;
}
# endif
#endif

int say_window_open(say_window *win, size_t w, size_t h, const char *title,
                    uint8_t style) {
  if (w < 1 || h < 1) {
    say_error_set("window size must be at least (1,1)");
    return 0;
  }

#ifdef SAY_OSX
  if (!say_imp_window_open(win->win, title, w, h, style))
    return false;
#else
  if (win->dis)
    say_window_close(win);

  win->show_cursor = true;

  win->dis = XOpenDisplay(NULL);

  if (!win->dis) {
    say_error_set("could not open X display");
    return 0;
  }

  win->screen_id = DefaultScreen(win->dis);

  say_window_find_config(win);

  int x = 0, y = 0;

  if (style & SAY_WINDOW_FULLSCREEN) {
    /* x = (DisplayWidth(win->dis,  win->screen_id) - w) / 2; */
    /* y = (DisplayHeight(win->dis, win->screen_id) - h) / 2; */

    if (!say_window_enable_fullscreen(win, w, h)) {
      say_window_close(win);
      return false;
    }
  }

  XSetWindowAttributes attributes;
  attributes.event_mask = say_event_mask;
  attributes.override_redirect = (style & SAY_WINDOW_FULLSCREEN) ? 1 : 0;
  attributes.colormap = XCreateColormap(win->dis,
                                        RootWindow(win->dis, win->screen_id),
                                        win->vi->visual, AllocNone);

  win->win = XCreateWindow(win->dis,
                           RootWindow(win->dis, win->screen_id),
                           x, y,
                           w, h,
                           0,
                           DefaultDepth(win->dis, win->screen_id),
                           InputOutput,
                           win->vi->visual,
                           CWEventMask | CWColormap | CWOverrideRedirect,
                           &attributes);

  if (!win->win) {
    say_error_set("could not open window");
    return 0;
  }

  if (title)
    XStoreName(win->dis, win->win, title);

  XClassHint hint;
  hint.res_name  = "ray";
  hint.res_class = "ray";

  XSetClassHint(win->dis, win->win, &hint);

  Atom win_type = XInternAtom(win->dis, "_NET_WM_WINDOW_TYPE", False);
  Atom win_normal = XInternAtom(win->dis, "_NET_WM_WINDOW_TYPE_NORMAL", False);
  XChangeProperty(win->dis, win->win, win_type, XA_ATOM, 32,
                  PropModeReplace,
                  (unsigned char *)&win_normal, 1);

  win->delete_event = XInternAtom(win->dis, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(win->dis, win->win, &win->delete_event, 1);

  if (!(style & SAY_WINDOW_RESIZABLE) && !(style && SAY_WINDOW_FULLSCREEN)) {
    XSizeHints hints;
    hints.flags      = PMinSize | PMaxSize;
    hints.min_width  = hints.max_width  = w;
    hints.min_height = hints.max_height = h;
    XSetWMNormalHints(win->dis, win->win, &hints);
  }

  Atom WMHintsAtom = XInternAtom(win->dis, "_MOTIF_WM_HINTS", 0);
  if (WMHintsAtom && !(style & SAY_WINDOW_FULLSCREEN)) {
    static const unsigned long MWM_HINTS_FUNCTIONS   = 1 << 0;
    static const unsigned long MWM_HINTS_DECORATIONS = 1 << 1;

    static const unsigned long MWM_DECOR_BORDER      = 1 << 1;
    static const unsigned long MWM_DECOR_RESIZEH     = 1 << 2;
    static const unsigned long MWM_DECOR_TITLE       = 1 << 3;
    static const unsigned long MWM_DECOR_MENU        = 1 << 4;
    static const unsigned long MWM_DECOR_MINIMIZE    = 1 << 5;
    static const unsigned long MWM_DECOR_MAXIMIZE    = 1 << 6;

    static const unsigned long MWM_FUNC_RESIZE       = 1 << 1;
    static const unsigned long MWM_FUNC_MOVE         = 1 << 2;
    static const unsigned long MWM_FUNC_MINIMIZE     = 1 << 3;
    static const unsigned long MWM_FUNC_MAXIMIZE     = 1 << 4;
    static const unsigned long MWM_FUNC_CLOSE        = 1 << 5;

    struct WMHints {
      unsigned long flags;
      unsigned long functions;
      unsigned long decorations;
      long          input_mode;
      unsigned long state;
    } hints;

    hints.flags       = MWM_HINTS_FUNCTIONS | MWM_HINTS_DECORATIONS;
    hints.decorations = 0;
    hints.functions   = 0;

    if (!(style & SAY_WINDOW_NO_FRAME)) {
      hints.decorations |= MWM_DECOR_BORDER | MWM_DECOR_TITLE |
        MWM_DECOR_MINIMIZE | MWM_DECOR_MENU;
      hints.functions   |= MWM_FUNC_MOVE | MWM_FUNC_MINIMIZE | MWM_FUNC_CLOSE;
    }

    if (style & SAY_WINDOW_RESIZABLE) {
      hints.decorations |= MWM_DECOR_MAXIMIZE | MWM_DECOR_RESIZEH;
      hints.functions   |= MWM_FUNC_MAXIMIZE | MWM_FUNC_RESIZE;
    }

    const unsigned char* ptr = (const unsigned char*)(&hints);
    XChangeProperty(win->dis, win->win, WMHintsAtom, WMHintsAtom, 32,
                    PropModeReplace, ptr, 5);
  }

  win->im = XOpenIM(win->dis, NULL, NULL, NULL);
  if (win->im) {
    win->ic = XCreateIC(win->im,
                        XNClientWindow, win->win,
                        XNFocusWindow, win->win,
                        XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
                        NULL);
  }

  if (style & SAY_WINDOW_FULLSCREEN) {
    Atom above = XInternAtom(win->dis, "_NET_WM_STATE_ABOVE", false);
    XChangeProperty(win->dis, win->win,
                    XInternAtom(win->dis, "_NET_WM_STATE", false),
                    XA_ATOM, 32, PropModeReplace,
                    (unsigned char *)&above, 1);


    XMoveResizeWindow(win->dis, win->win, 0, 0, w, h);
    XMapRaised(win->dis, win->win);

    XGrabPointer(win->dis, win->win, true, 0,
                 GrabModeAsync, GrabModeAsync,
                 win->win, None, CurrentTime);

    XGrabKeyboard(win->dis, win->win, true,
                  GrabModeAsync, GrabModeAsync, CurrentTime);
  }
  else
    XMapWindow(win->dis, win->win);

  XFlush(win->dis);
#endif

  say_target_set_context_proc(win->target,
                              (say_context_proc)say_context_create_for_window);
  say_target_set_bind_hook(win->target, say_window_unbind_fbo);
  say_target_set_custom_data(win->target, (void*)win);
  say_target_set_size(win->target, say_make_vector2(w, h));
  say_view_set_size(win->target->view, say_make_vector2(w, h));
  say_view_set_center(win->target->view, say_make_vector2(w / 2.0, h / 2.0));
  say_target_make_current(win->target);

  return 1;
}

void say_window_close(say_window *win) {
  say_target_set_context_proc(win->target, NULL);
  say_input_reset(&win->input);

#ifdef SAY_OSX
  say_imp_window_close(win->win);
#else
  win->cached_event.type = SAY_EVENT_NONE;

  if (win->dis && win->old_video_mode >= 0) {
# ifdef HAVE_XRANDR
    XRRScreenConfiguration *conf = XRRGetScreenInfo(win->dis,
                                                    RootWindow(win->dis,
                                                               win->screen_id));
    if (conf) {
      Rotation current_rot;
      XRRConfigCurrentConfiguration(conf, &current_rot);

      XRRSetScreenConfig(win->dis, conf,
                         RootWindow(win->dis, win->screen_id),
                         win->old_video_mode,
                         current_rot, CurrentTime);
      XRRFreeScreenConfigInfo(conf);
    }
# endif
  }

  if (win->hidden_cursor) {
    XFreeCursor(win->dis, win->hidden_cursor);
    win->hidden_cursor = None;
  }

  if (win->ic) {
    XDestroyIC(win->ic);
    win->ic = NULL;
  }

  if (win->im) {
    XCloseIM(win->im);
    win->im = NULL;
  }

  if (win->vi) {
    XFree(win->vi);
    win->vi = NULL;
  }

  if (win->win) {
    XDestroyWindow(win->dis, win->win);
    XFlush(win->dis);

    win->win = None;
  }

  if (win->dis) {
    XCloseDisplay(win->dis);
    win->dis = NULL;
  }
#endif
}

void say_window_update(say_window *win) {
  say_target_update(win->target);
}

void say_window_hide_cursor(say_window *win) {
#ifdef SAY_OSX
  say_imp_window_hide_cursor(win->win);
  win->show_cursor = false;
#else
  if (win->dis) {
    if (!win->hidden_cursor)
      say_window_build_cursor(win);

    XDefineCursor(win->dis, win->win, win->hidden_cursor);
    win->show_cursor = false;
  }
#endif
}

void say_window_show_cursor(say_window *win) {
#ifdef SAY_OSX
  say_imp_window_show_cursor(win->win);
  win->show_cursor = true;
#else
  if (win->dis) {
    XDefineCursor(win->dis, win->win, None);
    win->show_cursor = true;
  }
#endif
}

bool say_window_is_cursor_shown(say_window *win) {
  return win->show_cursor;
}

bool say_window_set_icon(say_window *win, say_image *icon) {
#ifdef SAY_OSX
  if (say_image_get_width(icon)  == 0 ||
      say_image_get_height(icon) == 0) {
    say_error_set("can't create icon from empty string");
    return false;
  }

  say_imp_window_set_icon(win->win, icon);
  return true;
#else
  if (!win->win) {
    say_error_set("window has not been opened");
    return false;
  }

  /* Convert image to BGRA */

  say_color *orig_buf = say_image_get_buffer(icon);
  long      *pixels   = malloc(sizeof(long) * (2 + (icon->width * icon->height)));

  if (!pixels) {
    say_error_set("could not allocate icon buffer");
    return false;
  }

  pixels[0] = icon->width;
  pixels[1] = icon->height;

  long *buf = &pixels[2];

  for (size_t n = 0; n < icon->height * icon->width; n++) {
    uint8_t *pixel = (uint8_t*)buf;

    pixel[0] = orig_buf[n].b;
    pixel[1] = orig_buf[n].g;
    pixel[2] = orig_buf[n].r;
    pixel[3] = orig_buf[n].a;

    buf++;
  }

  /* Set _NET_WM_ICON */

  Atom icon_atom = XInternAtom(win->dis, "_NET_WM_ICON", false);
  XChangeProperty(win->dis, win->win, icon_atom,
                  XA_CARDINAL, 32, PropModeReplace,
                  (uint8_t*)pixels,
                  2 + (icon->width * icon->height));
  XFlush(win->dis);

  free(pixels);

  return true;
#endif
}

void say_window_set_title(say_window *win, const char *title) {
#ifdef SAY_OSX
  say_imp_window_set_title(win->win, title);
#endif
}

void say_window_resize(say_window *win, size_t w, size_t h) {
#ifdef SAY_OSX
  say_imp_window_resize(win->win, w, h);
#endif
}

#ifdef SAY_X11
static say_button say_window_convert_button(unsigned int button) {
  switch (button) {
    case Button1: return SAY_BUTTON_LEFT;
    case Button2: return SAY_BUTTON_MIDDLE;
    case Button3: return SAY_BUTTON_RIGHT;
    default:      return SAY_BUTTON_UNKNOWN;
  }
}

static say_key say_window_convert_key_code(KeySym sym) {
  switch (sym) {
    case XK_Shift_L:   return SAY_KEY_LSHIFT;
    case XK_Control_L: return SAY_KEY_LCONTROL;
    case XK_Alt_L:     return SAY_KEY_LMETA;
    case XK_Super_L:   return SAY_KEY_LSUPER;

    case XK_Shift_R:   return SAY_KEY_RSHIFT;
    case XK_Control_R: return SAY_KEY_RCONTROL;
    case XK_Alt_R:     return SAY_KEY_RMETA;
    case XK_Super_R:   return SAY_KEY_RSUPER;

    case XK_Menu:         return SAY_KEY_MENU;
    case XK_Escape:       return SAY_KEY_ESCAPE;
    case XK_semicolon:    return SAY_KEY_SEMICOLON;
    case XK_slash:        return SAY_KEY_SLASH;
    case XK_equal:        return SAY_KEY_EQUAL;
    case XK_minus:        return SAY_KEY_MINUS;
    case XK_bracketleft:  return SAY_KEY_LBRACKET;
    case XK_bracketright: return SAY_KEY_RBRACKET;
    case XK_comma:        return SAY_KEY_COMMA;
    case XK_period:       return SAY_KEY_PERIOD;
    case XK_dead_acute:   return SAY_KEY_QUOTE;
    case XK_backslash:    return SAY_KEY_BACKSLASH;
    case XK_dead_grave:   return SAY_KEY_TILDE;
    case XK_space:        return SAY_KEY_SPACE;
    case XK_Return:       return SAY_KEY_RETURN;
    case XK_KP_Enter:     return SAY_KEY_RETURN;
    case XK_BackSpace:    return SAY_KEY_BACKSPACE;
    case XK_Tab:          return SAY_KEY_TAB;
    case XK_Prior:        return SAY_KEY_PAGE_UP;
    case XK_Next:         return SAY_KEY_PAGE_DOWN;
    case XK_End:          return SAY_KEY_END;
    case XK_Home:         return SAY_KEY_HOME;
    case XK_Insert:       return SAY_KEY_INSERT;
    case XK_Delete:       return SAY_KEY_DELETE;
    case XK_KP_Add:       return SAY_KEY_PLUS;
    case XK_KP_Subtract:  return SAY_KEY_MINUS;
    case XK_KP_Multiply:  return SAY_KEY_ASTERISK;
    case XK_KP_Divide:    return SAY_KEY_SLASH;
    case XK_Pause:        return SAY_KEY_PAUSE;

    case XK_F1:  return SAY_KEY_F1;
    case XK_F2:  return SAY_KEY_F2;
    case XK_F3:  return SAY_KEY_F3;
    case XK_F4:  return SAY_KEY_F4;
    case XK_F5:  return SAY_KEY_F5;
    case XK_F6:  return SAY_KEY_F6;
    case XK_F7:  return SAY_KEY_F7;
    case XK_F8:  return SAY_KEY_F8;
    case XK_F9:  return SAY_KEY_F9;
    case XK_F10: return SAY_KEY_F10;
    case XK_F11: return SAY_KEY_F11;
    case XK_F12: return SAY_KEY_F12;
    case XK_F13: return SAY_KEY_F13;
    case XK_F14: return SAY_KEY_F14;
    case XK_F15: return SAY_KEY_F15;

    case XK_Left:  return SAY_KEY_LEFT;
    case XK_Right: return SAY_KEY_RIGHT;
    case XK_Up:    return SAY_KEY_UP;
    case XK_Down:  return SAY_KEY_DOWN;

    case XK_KP_0: return SAY_KEY_KP_0;
    case XK_KP_1: return SAY_KEY_KP_1;
    case XK_KP_2: return SAY_KEY_KP_2;
    case XK_KP_3: return SAY_KEY_KP_3;
    case XK_KP_4: return SAY_KEY_KP_4;
    case XK_KP_5: return SAY_KEY_KP_5;
    case XK_KP_6: return SAY_KEY_KP_6;
    case XK_KP_7: return SAY_KEY_KP_7;
    case XK_KP_8: return SAY_KEY_KP_8;
    case XK_KP_9: return SAY_KEY_KP_9;

    case XK_A: return SAY_KEY_A;
    case XK_B: return SAY_KEY_B;
    case XK_C: return SAY_KEY_C;
    case XK_D: return SAY_KEY_D;
    case XK_E: return SAY_KEY_E;
    case XK_F: return SAY_KEY_F;
    case XK_G: return SAY_KEY_G;
    case XK_H: return SAY_KEY_H;
    case XK_I: return SAY_KEY_I;
    case XK_J: return SAY_KEY_J;
    case XK_K: return SAY_KEY_K;
    case XK_L: return SAY_KEY_L;
    case XK_M: return SAY_KEY_M;
    case XK_N: return SAY_KEY_N;
    case XK_O: return SAY_KEY_O;
    case XK_P: return SAY_KEY_P;
    case XK_Q: return SAY_KEY_Q;
    case XK_R: return SAY_KEY_R;
    case XK_S: return SAY_KEY_S;
    case XK_T: return SAY_KEY_T;
    case XK_U: return SAY_KEY_U;
    case XK_V: return SAY_KEY_V;
    case XK_W: return SAY_KEY_W;
    case XK_X: return SAY_KEY_X;
    case XK_Y: return SAY_KEY_Y;
    case XK_Z: return SAY_KEY_Z;

    case XK_0: return SAY_KEY_NUM_0;
    case XK_1: return SAY_KEY_NUM_1;
    case XK_2: return SAY_KEY_NUM_2;
    case XK_3: return SAY_KEY_NUM_3;
    case XK_4: return SAY_KEY_NUM_4;
    case XK_5: return SAY_KEY_NUM_5;
    case XK_6: return SAY_KEY_NUM_6;
    case XK_7: return SAY_KEY_NUM_7;
    case XK_8: return SAY_KEY_NUM_8;
    case XK_9: return SAY_KEY_NUM_9;

    default: return SAY_KEY_UNKNOWN;
  }
}

static int say_window_parse_event(say_window *win, say_event *ev) {
  XEvent xev;
  XNextEvent(win->dis, &xev);

  if (xev.xany.window != win->win)
    return 0;

  switch (xev.type) {
    case ClientMessage: { /* weird way to check for quit events */
      if ((xev.xclient.format == 32) &&
          ((Atom)xev.xclient.data.l[0] == win->delete_event)) {
        ev->type = SAY_EVENT_QUIT;
        return 1;
      }
    }

    case ConfigureNotify: {
      say_vector2 current_size = say_target_get_size(win->target);

      if (current_size.x != xev.xconfigure.width ||
          current_size.y != xev.xconfigure.height) {
        ev->type = SAY_EVENT_RESIZE;
        ev->ev.resize.size = say_make_vector2(xev.xconfigure.width, xev.xconfigure.height);
        return 1;
      }
      else
        return 0;
    }

    case MotionNotify: {
      ev->type = SAY_EVENT_MOUSE_MOTION;
      ev->ev.motion.pos = say_make_vector2(xev.xmotion.x, xev.xmotion.y);

      return 1;
    }

    case ButtonPress: {
      unsigned int button = xev.xbutton.button;
      if (button == Button4 || button == Button5)
        return 0;

      ev->type = SAY_EVENT_BUTTON_PRESS;
      ev->ev.button.pos = say_make_vector2(xev.xbutton.x, xev.xbutton.y);
      ev->ev.button.button = say_window_convert_button(button);

      return 1;
    }

    case ButtonRelease: {
      unsigned int button = xev.xbutton.button;
      if (button == Button4 || button == Button5) {
        ev->type = SAY_EVENT_WHEEL_MOTION;
        ev->ev.wheel.pos = say_make_vector2(xev.xbutton.x, xev.xbutton.y);
        ev->ev.wheel.delta = button == Button4 ? 1 : -1;
      }
      else {
        ev->type = SAY_EVENT_BUTTON_RELEASE;
        ev->ev.button.pos = say_make_vector2(xev.xbutton.x, xev.xbutton.y);
        ev->ev.button.button = say_window_convert_button(button);
      }

      return 1;
    }

    case EnterNotify: {
      ev->type = SAY_EVENT_MOUSE_ENTERED;
      return 1;
    }

    case LeaveNotify: {
      ev->type = SAY_EVENT_MOUSE_LEFT;
      return 1;
    }

    case FocusIn: {
      ev->type = SAY_EVENT_FOCUS_GAIN;
      return 1;
    }

    case FocusOut: {
      ev->type = SAY_EVENT_FOCUS_LOSS;
      return 1;
    }

    case KeyPress: {
      static XComposeStatus keyboard;
      char buffer[32];
      KeySym symbol;
      XLookupString(&xev.xkey, buffer, sizeof(buffer), &symbol, &keyboard);

      KeySym lower, upper;
      XConvertCase(symbol, &lower, &upper);

      symbol = upper;

      ev->type = SAY_EVENT_KEY_PRESS;
      ev->ev.key.code        = say_window_convert_key_code(symbol);
      ev->ev.key.native_code = symbol;

      /* is this a known key that's already pressed? */
      if (ev->ev.key.code != SAY_KEY_UNKNOWN &&
          win->input.keys[ev->ev.key.code - 1] == 1) {
        ev->type = SAY_EVENT_NONE;
      }

      ev->ev.key.mod = 0;
      if (xev.xkey.state & Mod1Mask)    ev->ev.key.mod |= SAY_MOD_META;
      if (xev.xkey.state & ControlMask) ev->ev.key.mod |= SAY_MOD_CONTROL;
      if (xev.xkey.state & ShiftMask)   ev->ev.key.mod |= SAY_MOD_SHIFT;
      if (xev.xkey.state & Mod4Mask)    ev->ev.key.mod |= SAY_MOD_SUPER;

      if (!XFilterEvent(&xev, None)) { /* if event shouldn't be discarded */
        /*
          Perform utf8 lookup if available and if an input context could be
          opened.
        */
#ifdef X_HAVE_UTF8_STRING
        if (win->ic) {
          Status status;
          uint8_t key_buffer[16];

          int length = Xutf8LookupString(win->ic, &xev.xkey, (char*)key_buffer,
                                         sizeof(key_buffer), NULL, &status);
          if (length > 0) {
            uint32_t character = say_utf8_to_utf32(key_buffer);
            win->cached_event.type = SAY_EVENT_TEXT_ENTERED;
            win->cached_event.ev.text.text = character;
          }
        }
        else
#endif
        {
          static XComposeStatus status;
          char key_buffer[16];

          if (XLookupString(&xev.xkey, key_buffer, sizeof(key_buffer), NULL,
                            &status)) {
            win->cached_event.type = SAY_EVENT_TEXT_ENTERED;
            win->cached_event.ev.text.text = (uint32_t)key_buffer[0];
          }
        }
      }

      if (ev->type == SAY_EVENT_NONE &&
          win->cached_event.type != SAY_EVENT_NONE) {
        *ev = win->cached_event;
        win->cached_event.type = SAY_EVENT_NONE;

        return 1;
      }
      else if (ev->type != SAY_EVENT_NONE)
        return 1;
      else
        return 0;
    }

    case KeyRelease: {
      /* Peek next event to know if this event should be discarded */
      if (XPending(win->dis)) {
        XEvent next_event;
        XPeekEvent(win->dis, &next_event);

        if (next_event.type == KeyPress &&
            next_event.xkey.time == xev.xkey.time &&
            next_event.xkey.keycode == xev.xkey.keycode) {
          return 0;
        }
      }

      char buffer[32];
      KeySym symbol;
      XLookupString(&xev.xkey, buffer, sizeof(buffer), &symbol, NULL);

      KeySym lower, upper;
      XConvertCase(symbol, &lower, &upper);

      symbol = upper;

      ev->type = SAY_EVENT_KEY_RELEASE;
      ev->ev.key.code        = say_window_convert_key_code(symbol);
      ev->ev.key.native_code = symbol;

      ev->ev.key.mod = 0;
      if (xev.xkey.state & Mod1Mask)    ev->ev.key.mod |= SAY_MOD_META;
      if (xev.xkey.state & ControlMask) ev->ev.key.mod |= SAY_MOD_CONTROL;
      if (xev.xkey.state & ShiftMask)   ev->ev.key.mod |= SAY_MOD_SHIFT;
      if (xev.xkey.state & Mod4Mask)    ev->ev.key.mod |= SAY_MOD_SUPER;

      if (ev->ev.key.code != SAY_KEY_UNKNOWN &&
          win->input.keys[ev->ev.key.code - 1] == 0) {
        ev->type = SAY_EVENT_NONE;
        return 0;
      }
      else
        return 1;
    }
  }

  return 0;
}
#endif

static void say_window_process_event(say_window *win, say_event *ev) {
  switch (ev->type) {
    case SAY_EVENT_KEY_PRESS: {
      say_input_press(&win->input, ev->ev.key.code);
      break;
    }

    case SAY_EVENT_KEY_RELEASE: {
      say_input_release(&win->input, ev->ev.key.code);
      break;
    }

    case SAY_EVENT_MOUSE_MOTION: {
      say_input_set_mouse_pos(&win->input, ev->ev.motion.pos);
      break;
    }

    case SAY_EVENT_RESIZE: {
      say_target_set_size(win->target, ev->ev.resize.size);
      break;
    }

#ifdef SAY_X11
    case SAY_EVENT_FOCUS_GAIN: {
      if (win->ic) XSetICFocus(win->ic);
      break;
    }

    case SAY_EVENT_FOCUS_LOSS: {
      if (win->ic) XUnsetICFocus(win->ic);
      break;
    }
#endif

    default: break; /* Prevent some warnings */
  }
}

int say_window_poll_event(say_window *win, say_event *ev) {
#ifdef SAY_OSX
  if (say_imp_window_poll_event(win->win, ev)) {
    say_window_process_event(win, ev);
    return true;
  }

  return false;
#else
  if (win->cached_event.type != SAY_EVENT_NONE) {
    say_window_process_event(win, &win->cached_event);

    *ev = win->cached_event;
    win->cached_event.type = SAY_EVENT_NONE;

    return 1;
  }

  ev->type = SAY_EVENT_NONE;

  if (!win->dis || !XPending(win->dis))
    return 0;

  do {
    if (say_window_parse_event(win, ev)) {
      say_window_process_event(win, ev);
      return 1;
    }
  } while (XPending(win->dis));

  return 0;
#endif
}

void say_window_wait_event(say_window *win, say_event *ev) {
#ifdef SAY_OSX
  say_imp_window_wait_event(win->win, ev);
  say_window_process_event(win, ev);
#else
  if (win->cached_event.type != SAY_EVENT_NONE) {
    *ev = win->cached_event;
    win->cached_event.type = SAY_EVENT_NONE;
    return;
  }

  ev->type = SAY_EVENT_NONE;

  if (!win->dis)
    return;

  while (1) {
    if (say_window_parse_event(win, ev)) {
      say_window_process_event(win, ev);
      return;
    }
  }
#endif
}

say_input *say_window_get_input(say_window *win) {
  return &win->input;
}
