#ifndef MAPVK_VK_TO_VSC
# define MAPVK_VK_TO_VSC 0
#endif

#ifndef VK_OEM_PLUS
# define VK_OEM_PLUS 0xBB
#endif

#ifndef VK_OEM_MINUS
# define VK_OEM_MINUS 0xBD
#endif

#ifndef VK_OEM_COMMA
# define VK_OEM_COMMA 0xBC
#endif

#ifndef VK_OEM_PERIOD
# define VK_OEM_PERIOD 0xBE
#endif

static DEVMODE say_win_get_mode() {
  DEVMODE mode;
  mode.dmSize = sizeof(DEVMODE);
  EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &mode);

  return mode;
}

size_t say_imp_screen_get_width() {
  return say_win_get_mode().dmPelsWidth;
}

size_t say_imp_screen_get_height() {
  return say_win_get_mode().dmPelsHeight;
}

static size_t say_window_count = 0;
static say_win_window *say_fullscreen_window = NULL;

static bool say_win_has_unicode() {
  static bool computed = false;
  static bool result   = false;

  if (!computed) {
    OSVERSIONINFO version;
    memset(&version, 0, sizeof(version));

    if (GetVersionEx(&version))
      result = version.dwPlatformId == VER_PLATFORM_WIN32_NT;
    else
      result = false;

    computed = true;
  }

  return result;
}

static uint8_t say_win_get_mod() {
  uint8_t mod = 0;

  if (HIWORD(GetAsyncKeyState(VK_MENU)))    mod |= SAY_MOD_META;
  if (HIWORD(GetAsyncKeyState(VK_CONTROL))) mod |= SAY_MOD_CONTROL;
  if (HIWORD(GetAsyncKeyState(VK_SHIFT)))   mod |= SAY_MOD_SHIFT;
  if (HIWORD(GetAsyncKeyState(VK_LWIN)))    mod |= SAY_MOD_SUPER;

  return mod;
}

static say_key say_win_get_key(WPARAM key, LPARAM flags) {
  switch (key) {
    case VK_SHIFT: {
      UINT lshift = MapVirtualKey(VK_LSHIFT, MAPVK_VK_TO_VSC);
      UINT code = (UINT)((flags & (0xFF << 16) >> 16));
      return code == lshift ? SAY_KEY_LSHIFT : SAY_KEY_RSHIFT;
    }

    case VK_MENU:
      return (HIWORD(flags) & KF_EXTENDED) ? SAY_KEY_RMETA : SAY_KEY_LMETA;
    case VK_CONTROL:
      return (HIWORD(flags) & KF_EXTENDED) ? SAY_KEY_RCONTROL : SAY_KEY_LCONTROL;

    case VK_LWIN: return SAY_KEY_LSUPER;
    case VK_RWIN: return SAY_KEY_RSUPER;

    case 'A': return SAY_KEY_A;
    case 'B': return SAY_KEY_B;
    case 'C': return SAY_KEY_C;
    case 'D': return SAY_KEY_D;
    case 'E': return SAY_KEY_E;
    case 'F': return SAY_KEY_F;
    case 'G': return SAY_KEY_G;
    case 'H': return SAY_KEY_H;
    case 'I': return SAY_KEY_I;
    case 'J': return SAY_KEY_J;
    case 'K': return SAY_KEY_K;
    case 'L': return SAY_KEY_L;
    case 'M': return SAY_KEY_M;
    case 'N': return SAY_KEY_N;
    case 'O': return SAY_KEY_O;
    case 'P': return SAY_KEY_P;
    case 'Q': return SAY_KEY_Q;
    case 'R': return SAY_KEY_R;
    case 'S': return SAY_KEY_S;
    case 'T': return SAY_KEY_T;
    case 'U': return SAY_KEY_U;
    case 'V': return SAY_KEY_V;
    case 'W': return SAY_KEY_W;
    case 'X': return SAY_KEY_X;
    case 'Y': return SAY_KEY_Y;
    case 'Z': return SAY_KEY_Z;

    case '0': return SAY_KEY_NUM_0;
    case '1': return SAY_KEY_NUM_1;
    case '2': return SAY_KEY_NUM_2;
    case '3': return SAY_KEY_NUM_3;
    case '4': return SAY_KEY_NUM_4;
    case '5': return SAY_KEY_NUM_5;
    case '6': return SAY_KEY_NUM_6;
    case '7': return SAY_KEY_NUM_7;
    case '8': return SAY_KEY_NUM_8;
    case '9': return SAY_KEY_NUM_9;

    case VK_NUMPAD0: return SAY_KEY_KP_0;
    case VK_NUMPAD1: return SAY_KEY_KP_1;
    case VK_NUMPAD2: return SAY_KEY_KP_2;
    case VK_NUMPAD3: return SAY_KEY_KP_3;
    case VK_NUMPAD4: return SAY_KEY_KP_4;
    case VK_NUMPAD5: return SAY_KEY_KP_5;
    case VK_NUMPAD6: return SAY_KEY_KP_6;
    case VK_NUMPAD7: return SAY_KEY_KP_7;
    case VK_NUMPAD8: return SAY_KEY_KP_8;
    case VK_NUMPAD9: return SAY_KEY_KP_9;

    case VK_LEFT:  return SAY_KEY_LEFT;
    case VK_RIGHT: return SAY_KEY_RIGHT;
    case VK_UP:    return SAY_KEY_UP;
    case VK_DOWN:  return SAY_KEY_DOWN;

    case VK_F1:  return SAY_KEY_F1;
    case VK_F2:  return SAY_KEY_F2;
    case VK_F3:  return SAY_KEY_F3;
    case VK_F4:  return SAY_KEY_F4;
    case VK_F5:  return SAY_KEY_F5;
    case VK_F6:  return SAY_KEY_F6;
    case VK_F7:  return SAY_KEY_F7;
    case VK_F8:  return SAY_KEY_F8;
    case VK_F9:  return SAY_KEY_F9;
    case VK_F10: return SAY_KEY_F10;
    case VK_F11: return SAY_KEY_F11;
    case VK_F12: return SAY_KEY_F12;
    case VK_F13: return SAY_KEY_F13;
    case VK_F14: return SAY_KEY_F14;
    case VK_F15: return SAY_KEY_F15;

    case VK_APPS:       return SAY_KEY_MENU;
    case VK_OEM_1:      return SAY_KEY_SEMICOLON;
    case VK_OEM_2:      return SAY_KEY_SLASH;
    case VK_OEM_PLUS:   return SAY_KEY_EQUAL; /* not a bug */
    case VK_OEM_MINUS:  return SAY_KEY_MINUS;
    case VK_OEM_4:      return SAY_KEY_LBRACKET;
    case VK_OEM_6:      return SAY_KEY_RBRACKET;
    case VK_OEM_COMMA:  return SAY_KEY_COMMA;
    case VK_OEM_PERIOD: return SAY_KEY_PERIOD;
    case VK_OEM_7:      return SAY_KEY_QUOTE;
    case VK_OEM_5:      return SAY_KEY_BACKSLASH;
    case VK_OEM_3:      return SAY_KEY_TILDE;
    case VK_ESCAPE:     return SAY_KEY_ESCAPE;
    case VK_SPACE:      return SAY_KEY_SPACE;
    case VK_RETURN:     return SAY_KEY_RETURN;
    case VK_BACK:       return SAY_KEY_BACKSPACE;
    case VK_TAB:        return SAY_KEY_TAB;
    case VK_PRIOR:      return SAY_KEY_PAGE_UP;
    case VK_NEXT:       return SAY_KEY_PAGE_DOWN;
    case VK_END:        return SAY_KEY_END;
    case VK_HOME:       return SAY_KEY_HOME;
    case VK_INSERT:     return SAY_KEY_INSERT;
    case VK_DELETE:     return SAY_KEY_DELETE;
    case VK_ADD:        return SAY_KEY_PLUS;
    case VK_SUBTRACT:   return SAY_KEY_MINUS;
    case VK_MULTIPLY:   return SAY_KEY_ASTERISK;
    case VK_DIVIDE:     return SAY_KEY_SLASH;
    case VK_PAUSE:      return SAY_KEY_PAUSE;

    default: return SAY_KEY_UNKNOWN;
  }
}

static void say_win_window_translate(say_win_window *win, UINT msg, WPARAM wparam,
                                     LPARAM lparam) {
  if (!win->win) return;

  switch (msg) {
    case WM_SETCURSOR: {
      if (LOWORD(lparam) == HTCLIENT)
        SetCursor(win->cursor);
      break;
    }

    case WM_CLOSE: {
      say_event ev;
      ev.type = SAY_EVENT_QUIT;
      say_array_push(win->events, &ev);
      break;
    }

    case WM_SIZE: {
      if (wparam != SIZE_MINIMIZED) {
        RECT rect;
        GetClientRect(win->win, &rect);
        size_t w = rect.right  - rect.left;
        size_t h = rect.bottom - rect.top;

        say_event ev;
        ev.type = SAY_EVENT_RESIZE;
        ev.ev.resize.size = say_make_vector2(w, h);
        say_array_push(win->events, &ev);
      }

      break;
    }

    case WM_SETFOCUS: {
      say_event ev;
      ev.type = SAY_EVENT_FOCUS_GAIN;
      say_array_push(win->events, &ev);
      break;
    }

    case WM_KILLFOCUS: {
      say_event ev;
      ev.type = SAY_EVENT_FOCUS_LOSS;
      say_array_push(win->events, &ev);
      break;
    }

    case WM_CHAR: {
      say_event ev;
      ev.type = SAY_EVENT_TEXT_ENTERED;
      ev.ev.text.text = (uint32_t)wparam;

      say_array_push(win->events, &ev);

      break;
    }

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN: {
      if ((HIWORD(lparam) & KF_REPEAT) == 0) {
        say_event ev;
        ev.type = SAY_EVENT_KEY_PRESS;
        ev.ev.key.mod         = say_win_get_mod();
        ev.ev.key.native_code = (uint32_t)wparam;
        ev.ev.key.code        = say_win_get_key(wparam, lparam);

        say_array_push(win->events, &ev);
      }
      break;
    }

    case WM_KEYUP:
    case WM_SYSKEYUP: {
      say_event ev;
      ev.type = SAY_EVENT_KEY_RELEASE;
      ev.ev.key.mod         = say_win_get_mod();
      ev.ev.key.native_code = (uint32_t)wparam;
      ev.ev.key.code        = say_win_get_key(wparam, lparam);

      say_array_push(win->events, &ev);
      break;
    }

    case WM_MOUSEWHEEL: {
      POINT pos;
      pos.x = LOWORD(lparam);
      pos.y = HIWORD(lparam);
      ScreenToClient(win->win, &pos);

      say_event ev;
      ev.type = SAY_EVENT_WHEEL_MOTION;
      ev.ev.wheel.pos   = say_make_vector2(pos.x, pos.y);
      ev.ev.wheel.delta = (int16_t)(HIWORD(wparam)) / 120;

      say_array_push(win->events, &ev);
      break;
    }

    case WM_LBUTTONDOWN: {
      say_event ev;
      ev.type = SAY_EVENT_BUTTON_PRESS;
      ev.ev.button.pos = say_make_vector2(LOWORD(lparam), HIWORD(lparam));
      ev.ev.button.button = SAY_BUTTON_LEFT;

      say_array_push(win->events, &ev);
      break;
    }

    case WM_LBUTTONUP: {
      say_event ev;
      ev.type = SAY_EVENT_BUTTON_RELEASE;
      ev.ev.button.pos = say_make_vector2(LOWORD(lparam), HIWORD(lparam));
      ev.ev.button.button = SAY_BUTTON_LEFT;

      say_array_push(win->events, &ev);
      break;
    }

    case WM_RBUTTONDOWN: {
      say_event ev;
      ev.type = SAY_EVENT_BUTTON_PRESS;
      ev.ev.button.pos = say_make_vector2(LOWORD(lparam), HIWORD(lparam));
      ev.ev.button.button = SAY_BUTTON_RIGHT;

      say_array_push(win->events, &ev);
      break;
    }

    case WM_RBUTTONUP: {
      say_event ev;
      ev.type = SAY_EVENT_BUTTON_RELEASE;
      ev.ev.button.pos = say_make_vector2(LOWORD(lparam), HIWORD(lparam));
      ev.ev.button.button = SAY_BUTTON_RIGHT;

      say_array_push(win->events, &ev);
      break;
    }

    case WM_MBUTTONDOWN: {
      say_event ev;
      ev.type = SAY_EVENT_BUTTON_PRESS;
      ev.ev.button.pos = say_make_vector2(LOWORD(lparam), HIWORD(lparam));
      ev.ev.button.button = SAY_BUTTON_MIDDLE;

      say_array_push(win->events, &ev);
      break;
    }

    case WM_MBUTTONUP: {
      say_event ev;
      ev.type = SAY_EVENT_BUTTON_RELEASE;
      ev.ev.button.pos = say_make_vector2(LOWORD(lparam), HIWORD(lparam));
      ev.ev.button.button = SAY_BUTTON_MIDDLE;

      say_array_push(win->events, &ev);
      break;
    }

    case WM_MOUSEMOVE: {
      if (!win->cursor_inside) {
        win->cursor_inside = true;

        TRACKMOUSEEVENT mouse_ev;
        mouse_ev.cbSize    = sizeof(mouse_ev);
        mouse_ev.hwndTrack = win->win;
        mouse_ev.dwFlags   = TME_LEAVE;
        TrackMouseEvent(&mouse_ev);

        say_event ev;
        ev.type = SAY_EVENT_MOUSE_ENTERED;
        say_array_push(win->events, &ev);
      }

      say_event ev;
      ev.type = SAY_EVENT_MOUSE_MOTION;
      ev.ev.motion.pos = say_make_vector2(LOWORD(lparam), HIWORD(lparam));

      say_array_push(win->events, &ev);
      break;
    }

    case WM_MOUSELEAVE: {
      win->cursor_inside = false;

      say_event ev;
      ev.type = SAY_EVENT_MOUSE_LEFT;
      say_array_push(win->events, &ev);
      break;
    }
  }
}

static LRESULT CALLBACK say_win_callback(HWND hwnd, UINT msg, WPARAM wparam,
                                         LPARAM lparam) {
  if (msg == WM_CREATE) {
    LONG_PTR win = (LONG_PTR)((CREATESTRUCT*)lparam)->lpCreateParams;
    SetWindowLongPtr(hwnd, GWLP_USERDATA, win);
  }

  say_win_window *win = (say_win_window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
  if (win) {
    say_win_window_translate(win, msg, wparam, lparam);
  }

  /* Stop here for close messages, so we can keep the window open */
  if (msg == WM_CLOSE)
    return 0;

  /* Forward other messages to the OS */
  if (say_win_has_unicode())
    return DefWindowProcW(hwnd, msg, wparam, lparam);
  else
    return DefWindowProcA(hwnd, msg, wparam, lparam);
}

static void say_win_register_class() {
  if (say_win_has_unicode()) {
    WNDCLASSW klass;
    klass.style         = 0;
    klass.lpfnWndProc   = say_win_callback;
    klass.cbClsExtra    = 0;
    klass.cbWndExtra    = 0;
    klass.hInstance     = GetModuleHandle(NULL);
    klass.hIcon         = NULL;
    klass.hCursor       = 0;
    klass.hbrBackground = 0;
    klass.lpszMenuName  = NULL;
    klass.lpszClassName = L"ray";

    RegisterClassW(&klass);
  }
  else {
    WNDCLASSA klass;
    klass.style         = 0;
    klass.lpfnWndProc   = say_win_callback;
    klass.cbClsExtra    = 0;
    klass.cbWndExtra    = 0;
    klass.hInstance     = GetModuleHandle(NULL);
    klass.hIcon         = NULL;
    klass.hCursor       = 0;
    klass.hbrBackground = 0;
    klass.lpszMenuName  = NULL;
    klass.lpszClassName = "ray";

    RegisterClassA(&klass);
  }
}

bool say_win_window_start_fullscreen(say_win_window *win, size_t w, size_t h) {
  DEVMODE mode;
  mode.dmSize       = sizeof(mode);
  mode.dmPelsWidth  = w;
  mode.dmPelsHeight = h;
  mode.dmBitsPerPel = 32;
  mode.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;

  if (ChangeDisplaySettings(&mode, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
    say_error_set("could not create fullscreen window");
    return false;
  }

  SetWindowLong(win->win, GWL_STYLE, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
  SetWindowLong(win->win, GWL_EXSTYLE, WS_EX_APPWINDOW);

  SetWindowPos(win->win, HWND_TOP, 0, 0, w, h, SWP_FRAMECHANGED);
  ShowWindow(win->win, SW_SHOW);

  say_fullscreen_window = win;

  return true;
}

say_imp_window say_imp_window_create() {
  say_win_window *win = malloc(sizeof(say_win_window));

  win->win    = 0;
  win->icon   = 0;
  win->cursor = LoadCursor(NULL, IDC_ARROW);;

  win->events = say_array_create(sizeof(say_event), NULL, NULL);

  return win;
}

void say_imp_window_free(say_imp_window win) {
  say_imp_window_close(win);
  say_array_free(win->events);
  free(win);
}

bool say_imp_window_open(say_imp_window win, const char *title,
                         size_t w, size_t h,
                         uint8_t style) {
  say_imp_window_close(win);

  win->cursor_inside = false;

  if (say_window_count == 0) {
    say_win_register_class();
  }

  say_window_count++;

  HDC dc = GetDC(NULL);
  int x = (GetDeviceCaps(dc, HORZRES) - w) / 2;
  int y = (GetDeviceCaps(dc, VERTRES) - h) / 2;
  ReleaseDC(NULL, dc);

  size_t real_w = 0, real_h = 0;

  DWORD win_style = WS_VISIBLE;
  if (!(style & SAY_WINDOW_FULLSCREEN)) {
    if (style & SAY_WINDOW_NO_FRAME) {
      win_style |= WS_POPUP;
    }
    else {
      win_style |= WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
      if (style & SAY_WINDOW_RESIZABLE) {
        win_style |= WS_THICKFRAME | WS_MAXIMIZEBOX;
      }
    }

    RECT rect = {0, 0, w, h};
    AdjustWindowRect(&rect, win_style, false);
    real_w = rect.right  - rect.left;
    real_h = rect.bottom - rect.top;
  }

  if (say_win_has_unicode()) {
    wchar_t win_title[256];
    int count = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, title,
                                    strlen(title), win_title, 255);
    win_title[count] = L'\0';
    win->win = CreateWindowW(L"ray", win_title, win_style, x, y, real_w, real_h,
                             NULL, NULL, GetModuleHandle(NULL), win);
  }
  else {
    win->win = CreateWindowA("ray", title, win_style, x, y, real_w, real_h,
                             NULL, NULL, GetModuleHandle(NULL), win);
  }

  if (style & SAY_WINDOW_FULLSCREEN) {
    return say_win_window_start_fullscreen(win, w, h);
  }
  else
    return true;
}

void say_imp_window_close(say_imp_window win) {
  if (say_fullscreen_window == win) {
    ChangeDisplaySettings(NULL, 0);
    say_fullscreen_window = NULL;
  }

  if (win->icon) {
    DestroyIcon(win->icon);
    win->icon = 0;
  }

  if (!win->win)
    return;

  DestroyWindow(win->win);

  if (!--say_window_count) {
    if (say_win_has_unicode())
      UnregisterClassW(L"ray", GetModuleHandle(NULL));
    else
      UnregisterClassA("ray", GetModuleHandle(NULL));
  }
}

void say_imp_window_show_cursor(say_imp_window win) {
  win->cursor = LoadCursor(NULL, IDC_ARROW);
  SetCursor(win->cursor);
}

void say_imp_window_hide_cursor(say_imp_window win) {
  win->cursor = NULL;
  SetCursor(NULL);
}

bool say_imp_window_set_icon(say_imp_window win, struct say_image *img) {
  if (!win->win) {
    say_error_set("window isn't open");
    return false;
  }

  if (win->icon) {
    DestroyIcon(win->icon);
    win->icon = 0;
  }

  size_t w = say_image_get_width(img), h = say_image_get_height(img);
  uint8_t *buf = malloc(w * h * 4);

  if (!buf) {
    say_error_set("could not allocate icon buffer");
    return false;
  }

  say_color *orig = say_flip_color_buffer_copy(say_image_get_buffer(img),
                                               w, h);

  for (size_t i = 0; i < w * h; i++) {
    buf[i * 4 + 0] =  orig[i].b;
    buf[i * 4 + 1] =  orig[i].g;
    buf[i * 4 + 2] =  orig[i].r;
    buf[i * 4 + 3] =  orig[i].a;
  }

  free(orig);

  win->icon = CreateIcon(GetModuleHandle(NULL), w, h, 1, 32, NULL, buf);

  if (!win->icon) {
    free(buf);
    say_error_set("could not create icon");
    return false;
  }

  SendMessage(win->win, WM_SETICON, ICON_BIG,   (LPARAM)win->icon);
  SendMessage(win->win, WM_SETICON, ICON_SMALL, (LPARAM)win->icon);

  return true;
}

void say_imp_window_set_title(say_imp_window win, const char *title) {
  SetWindowText(win->win, title);
}

bool say_imp_window_resize(say_imp_window win, size_t w, size_t h) {
  if (!win->win) {
    say_error_set("window is not opened");
    return false;
  }

  RECT rect = {0, 0, w, h};
  AdjustWindowRect(&rect, GetWindowLong(win->win, GWL_STYLE), false);
  size_t real_w = rect.right  - rect.left;
  size_t real_h = rect.bottom - rect.top;

  SetWindowPos(win->win, NULL, 0, 0, real_w, real_h, SWP_NOMOVE | SWP_NOZORDER);

  return true;
}

bool say_imp_window_poll_event(say_imp_window win, struct say_event *ev,
                               struct say_input *input) {

  if (say_array_get_size(win->events) == 0) {
    MSG message;
    while (PeekMessage(&message, win->win, 0, 0, PM_REMOVE)) {
      TranslateMessage(&message);
      DispatchMessage(&message);
    }
  }

  if (say_array_get_size(win->events) == 0) {
    ev->type = SAY_EVENT_NONE;
    return false;
  }
  else {
    *ev = *(say_event*)say_array_get(win->events, 0);
    say_array_delete(win->events, 0);
    return true;
  }
}

void say_imp_window_wait_event(say_imp_window win, struct say_event *ev,
                               struct say_input *input) {
  while (say_array_get_size(win->events) == 0) {
    WaitMessage();

    MSG message;
    while (PeekMessage(&message, win->win, 0, 0, PM_REMOVE)) {
      TranslateMessage(&message);
      DispatchMessage(&message);
    }
  }

  *ev = *(say_event*)say_array_get(win->events, 0);
  say_array_delete(win->events, 0);
}
