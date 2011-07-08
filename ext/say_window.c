#include "say.h"

static void say_window_unbind_fbo(void *window) {
  say_image_target_unbind();
}

#ifdef SAY_OSX
# include "say_osx_window.h"
#endif

#ifdef SAY_X11
# include "say_x11_window.h"
#endif

say_window *say_window_create() {
  say_window *win = (say_window*)malloc(sizeof(say_window));

  win->target = say_target_create();
  say_input_reset(&win->input);

  win->show_cursor = true;

  win->win = say_imp_window_create();

  return win;
}

void say_window_free(say_window *win) {
  say_window_close(win);
  say_target_free(win->target);

  say_imp_window_free(win->win);

  free(win);
}

int say_window_open(say_window *win, size_t w, size_t h, const char *title,
                    uint8_t style) {
  if (w < 1 || h < 1) {
    say_error_set("window size must be at least (1,1)");
    return 0;
  }

  win->show_cursor = true;

  if (!say_imp_window_open(win->win, title, w, h, style))
    return false;

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

  say_imp_window_close(win->win);
}

void say_window_update(say_window *win) {
  say_target_update(win->target);
}

void say_window_hide_cursor(say_window *win) {
  say_imp_window_hide_cursor(win->win);
  win->show_cursor = false;
}

void say_window_show_cursor(say_window *win) {
  say_imp_window_show_cursor(win->win);
  win->show_cursor = true;
}

bool say_window_is_cursor_shown(say_window *win) {
  return win->show_cursor;
}

bool say_window_set_icon(say_window *win, say_image *icon) {
  if (say_image_get_width(icon)  == 0 ||
      say_image_get_height(icon) == 0) {
    say_error_set("can't create icon from empty string");
    return false;
  }

  return say_imp_window_set_icon(win->win, icon);
}

void say_window_set_title(say_window *win, const char *title) {
  say_imp_window_set_title(win->win, title);
}

bool say_window_resize(say_window *win, size_t w, size_t h) {
  if (w == 0 || h == 0) {
    say_error_set("can't create empty window");
    return false;
  }

  return say_imp_window_resize(win->win, w, h);
}

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

    default: break; /* Prevent some warnings */
  }
}

int say_window_poll_event(say_window *win, say_event *ev) {
  if (say_imp_window_poll_event(win->win, ev, &win->input)) {
    say_window_process_event(win, ev);
    return true;
  }

  return false;
}

void say_window_wait_event(say_window *win, say_event *ev) {
  say_imp_window_wait_event(win->win, ev, &win->input);
  say_window_process_event(win, ev);
}

say_input *say_window_get_input(say_window *win) {
  return &win->input;
}
