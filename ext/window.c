#include "ray.h"

VALUE ray_cWindow = Qnil;

say_window *ray_rb2window(VALUE obj) {
  if (!RAY_IS_A(obj, rb_path2class("Ray::Window"))) {
    rb_raise(rb_eTypeError, "Can't convert %s into Ray::Window",
             RAY_OBJ_CLASSNAME(obj));
  }

  say_window *win;
  Data_Get_Struct(obj, say_window, win);

  return win;
}

static
VALUE ray_window_alloc(VALUE self) {
  say_window *obj = say_window_create();
  return Data_Wrap_Struct(self, NULL, say_window_free, obj);
}

/*
  @overload open(size, opts = {})
    @param [Ray::Vector2] size Size of the window

    @option opts [true, false] :resizable (false) whether the window can be
      resized.
    @option opts [true, false] :no_frame (false) whether the window should be
      decorated.
    @option opts [true, false] :fullscreen (false) whether the window should be
      fullscreen.
*/
static
VALUE ray_window_open(int argc, VALUE *argv, VALUE self) {
  VALUE title = Qnil, size = Qnil, opts = Qnil;
  rb_scan_args(argc, argv, "21", &title, &size, &opts);

  say_window *window = ray_rb2window(self);

  uint8_t flags = 0;
  if (!NIL_P(opts)) {
    if (!NIL_P(rb_hash_aref(opts, RAY_SYM("resizable"))))
      flags |= SAY_WINDOW_RESIZABLE;

    if (!NIL_P(rb_hash_aref(opts, RAY_SYM("no_frame"))))
      flags |= SAY_WINDOW_NO_FRAME;

    if (!NIL_P(rb_hash_aref(opts, RAY_SYM("fullscreen"))))
      flags |= SAY_WINDOW_FULLSCREEN;
  }

  say_vector2 c_size = ray_convert_to_vector2(size);

  if (!say_window_open(window, c_size.x, c_size.y,
                       StringValuePtr(title), flags)) {
    rb_raise(rb_eRuntimeError, "%s", say_error_get_last());
  }

  return self;
}

/* Closes the window */
static
VALUE ray_window_close(VALUE self) {
  say_window_close(ray_rb2window(self));
  return self;
}

/*
  @overload icon=(icon)
    Sets the icon used by the window
    @param [Ray::Image] icon Icon to use
*/
static
VALUE ray_window_set_icon(VALUE self, VALUE icon) {
  if (!say_window_set_icon(ray_rb2window(self), ray_rb2image(icon))) {
    rb_raise(rb_eRuntimeError, "%s", say_error_get_last());
  }

  return icon;
}

/*
  Updates the content of the window. Must be updated at the end of each frame.
*/
static
VALUE ray_window_update(VALUE self) {
  say_window_update(ray_rb2window(self));
  return self;
}

/* Shows the window cursor */
static
VALUE ray_window_show_cursor(VALUE self) {
  say_window_show_cursor(ray_rb2window(self));
  return self;
}

/* Hides the window cursor */
static
VALUE ray_window_hide_cursor(VALUE self) {
  say_window_hide_cursor(ray_rb2window(self));
  return self;
}

/*
  @overload poll_event(ev)
    Gets the next event from the event queue. If there's none, return directly.
    @param [Ray::Event] ev Event object to store the result into.
*/
static
VALUE ray_window_poll_event(VALUE self, VALUE ev) {
  say_window_poll_event(ray_rb2window(self), ray_rb2event(ev));
  return ev;
}

/*
  @overload wait_event(ev)
    Gets the next event from the event queue. If there's none, waits until
    there's one. Returns directly if the window is closed.

    @param [Ray::Event] ev Event object to store the result into.
*/
static
VALUE ray_window_wait_event(VALUE self, VALUE ev) {
  say_window_wait_event(ray_rb2window(self), ray_rb2event(ev));
  return ev;
}

/* @return [Ray::Input] The input used by this object */
static
VALUE ray_window_input(VALUE self) {
  return ray_input2rb(say_window_get_input(ray_rb2window(self)), self);
}

void Init_ray_window() {
  ray_cWindow = rb_define_class_under(ray_mRay, "Window", ray_cTarget);
  rb_define_alloc_func(ray_cWindow, ray_window_alloc);

  rb_define_method(ray_cWindow, "open", ray_window_open, -1);
  rb_define_method(ray_cWindow, "close", ray_window_close, 0);

  rb_define_method(ray_cWindow, "update", ray_window_update, 0);

  rb_define_method(ray_cWindow, "hide_cursor", ray_window_hide_cursor, 0);
  rb_define_method(ray_cWindow, "show_cursor", ray_window_show_cursor, 0);

  rb_define_method(ray_cWindow, "icon=", ray_window_set_icon, 1);

  rb_define_method(ray_cWindow, "poll_event", ray_window_poll_event, 1);
  rb_define_method(ray_cWindow, "wait_event", ray_window_wait_event, 1);

  rb_define_method(ray_cWindow, "input", ray_window_input, 0);
}
