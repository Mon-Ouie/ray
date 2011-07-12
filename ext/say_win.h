#include <windows.h>
#include <windef.h>
#include <process.h>

#include <stdbool.h>

struct say_array;

typedef struct say_win_window {
  HWND    win;
  HICON   icon;
  HCURSOR cursor;
  bool    cursor_inside;
  
  struct say_array *events;
} say_win_window;

typedef struct say_win_context {
  HGLRC context;
  HWND  win;
  HDC   device;
  
  bool owns_window;
} say_win_context;
