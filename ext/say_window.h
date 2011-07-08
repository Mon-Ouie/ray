#ifndef SAY_WINDOW_H_
#define SAY_WINDOW_H_

#include "say_target.h"
#include "say_event.h"
#include "say_image.h"

#define SAY_WINDOW_RESIZABLE  0x1
#define SAY_WINDOW_NO_FRAME   0x2
#define SAY_WINDOW_FULLSCREEN 0x4

typedef struct say_window {
  say_target *target;
  say_input input;

  bool show_cursor;

  say_imp_window win;
} say_window;

say_window *say_window_create();
void say_window_free(say_window *window);

int say_window_open(say_window *win, size_t w, size_t h, const char *title,
                    uint8_t style);
void say_window_close(say_window *win);

void say_window_update(say_window *win);

void say_window_hide_cursor(say_window *win);
void say_window_show_cursor(say_window *win);

bool say_window_set_icon(say_window *win, say_image *icon);

void say_window_set_title(say_window *win, const char *title);
bool  say_window_resize(say_window *win, size_t w, size_t h);

int  say_window_poll_event(say_window *win, say_event *ev);
void say_window_wait_event(say_window *win, say_event *ev);

say_input *say_window_get_input(say_window *win);

#endif
