#ifndef SAY_IMP_H_
#define SAY_IMP_H_

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef SAY_OSX
@class SayWindow;
@class SayContext;

typedef SayWindow  *say_imp_window;
typedef SayContext *say_imp_context;
#endif

#ifdef SAY_X11
struct say_x11_window;
struct say_x11_context;

typedef struct say_x11_window  *say_imp_window;
typedef struct say_x11_context *say_imp_context;
#endif

struct say_event;
struct say_input;
struct say_image;

size_t say_imp_screen_get_width();
size_t say_imp_screen_get_height();

say_imp_window say_imp_window_create();
void say_imp_window_free(say_imp_window win);

bool say_imp_window_open(say_imp_window win, const char *title,
                         size_t w, size_t h,
                         uint8_t flags);
void say_imp_window_close(say_imp_window win);

void say_imp_window_show_cursor(say_imp_window win);
void say_imp_window_hide_cursor(say_imp_window win);

bool say_imp_window_set_icon(say_imp_window win, struct say_image *img);

void say_imp_window_set_title(say_imp_window win, const char *title);
bool say_imp_window_resize(say_imp_window win, size_t w, size_t h);

bool say_imp_window_poll_event(say_imp_window win, struct say_event *ev,
                               struct say_input *input);
void say_imp_window_wait_event(say_imp_window win, struct say_event *ev,
                               struct say_input *input);

void say_imp_context_free(say_imp_context ctxt);

say_imp_context say_imp_context_create();
say_imp_context say_imp_context_create_shared(say_imp_context shared);
say_imp_context  say_imp_context_create_for_window(say_imp_context shared,
                                                   say_imp_window  win);

void say_imp_context_make_current(say_imp_context ctxt);
void say_imp_context_update(say_imp_context ctxt);

#endif
