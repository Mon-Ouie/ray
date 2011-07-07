#ifndef SAY_IMP_H_
#define SAY_IMP_H_

#ifdef SAY_OSX
typedef SayWindow  *say_imp_window;
typedef SayContext *say_imp_context;
#endif

#ifdef SAY_X11
#endif

struct say_event;
struct say_image;

say_imp_window say_imp_window_create();
void say_imp_window_free(say_imp_window win);

bool say_imp_window_open(say_imp_window win, const char *title,
                         size_t w, size_t h,
                         uint8_t flags);
void say_imp_window_close(say_imp_window win);

void say_imp_window_show_cursor(say_imp_window win);
void say_imp_window_hide_cursor(say_imp_window win);

void say_imp_window_set_icon(say_imp_window win, struct say_image *img);

bool say_imp_window_poll_event(say_imp_window win, struct say_event *ev);
void say_imp_window_wait_event(say_imp_window win, struct say_event *ev);

void say_imp_context_free(say_imp_context ctxt);

say_imp_context say_imp_context_create();
say_imp_context say_imp_context_create_shared(say_imp_context shared);
say_imp_context  say_imp_context_create_for_window(say_imp_context shared,
                                                   say_imp_window  win);

void say_imp_context_make_current(say_imp_context ctxt);
void say_imp_context_update(say_imp_context ctxt);

#endif
