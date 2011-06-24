#include "ray.h"

VALUE ray_cEvent = Qnil;

say_event *ray_rb2event(VALUE object) {
  if (!RAY_IS_A(object, rb_path2class("Ray::Event"))) {
    rb_raise(rb_eTypeError, "Can't convert %s into Ray::Event",
             RAY_OBJ_CLASSNAME(object));
  }

  say_event *ret = NULL;
  Data_Get_Struct(object, say_event, ret);

  return ret;
}

static
VALUE ray_event_alloc(VALUE self) {
  say_event *ev = malloc(sizeof(say_event));
  return Data_Wrap_Struct(self, NULL, free, ev);
}

/*
  @return [Integer] The type of the event. See Type* constants.
*/
static
VALUE ray_event_type(VALUE self) {
  return INT2FIX(ray_rb2event(self)->type);
}

/*
  @return [Ray::Vector2, nil] The position of the mouse indicated by the
    event. This is avairable to mouse motions, wheel motions, button presses,
    and button releases.
*/
static
VALUE ray_event_mouse_pos(VALUE self) {
  say_event *ev = ray_rb2event(self);

  if (ev->type == SAY_EVENT_MOUSE_MOTION)
    return ray_vector2_to_rb(ev->ev.motion.pos);
  else if (ev->type == SAY_EVENT_WHEEL_MOTION)
    return ray_vector2_to_rb(ev->ev.wheel.pos);
  else if (ev->type == SAY_EVENT_BUTTON_PRESS ||
           ev->type == SAY_EVENT_BUTTON_RELEASE)
    return ray_vector2_to_rb(ev->ev.button.pos);

  return Qnil;
}

/*
  @return [Integer, nil] How much the wheel moved. a positive number means it
    moved up.
*/
static
VALUE ray_event_wheel_delta(VALUE self) {
  say_event *ev = ray_rb2event(self);

  if (ev->type == SAY_EVENT_WHEEL_MOTION)
    return INT2FIX(ev->ev.wheel.delta);

  return Qnil;
}

/*
  @return [Integer, nil] The button that was pressed
*/
static
VALUE ray_event_button(VALUE self) {
  say_event *ev = ray_rb2event(self);

  if (ev->type == SAY_EVENT_BUTTON_PRESS ||
      ev->type == SAY_EVENT_BUTTON_RELEASE)
    return INT2FIX(ev->ev.button.button);

  return Qnil;
}

/*
  @return [String, nil] The text entered by the user.
*/
static
VALUE ray_event_text(VALUE self) {
  say_event *ev = ray_rb2event(self);

  if (ev->type == SAY_EVENT_TEXT_ENTERED) {
    char *ptr = (char*)&(ev->ev.text.text);
    return rb_str_new(ptr, sizeof(ev->ev.text.text));
  }

  return Qnil;
}

/*
  @return [Integer, nil] The code of the key that was pressed.
*/
static
VALUE ray_event_key(VALUE self) {
  say_event *ev = ray_rb2event(self);

  if (ev->type == SAY_EVENT_KEY_PRESS ||
      ev->type == SAY_EVENT_KEY_RELEASE) {
    return INT2FIX(ev->ev.key.code);
  }

  return Qnil;
}

/*
  @return [Integer, nil] The system-specific code of the key. This allows a
    program to bind an unknown key to a method by remembering this code.
*/
static
VALUE ray_event_native_key(VALUE self) {
  say_event *ev = ray_rb2event(self);

  if (ev->type == SAY_EVENT_KEY_PRESS ||
      ev->type == SAY_EVENT_KEY_RELEASE) {
    return INT2FIX(ev->ev.key.native_code);
  }

  return Qnil;
}


/*
  @return [Integer, nil] OR'd set of mod flags.
*/
static
VALUE ray_event_key_mod(VALUE self) {
  say_event *ev = ray_rb2event(self);

  if (ev->type == SAY_EVENT_KEY_PRESS ||
      ev->type == SAY_EVENT_KEY_RELEASE) {
    return INT2FIX(ev->ev.key.mod);
  }

  return Qnil;
}

/* @return [Ray::Vector2, nil] The new size of the window */
static
VALUE ray_event_window_size(VALUE self) {
  say_event *ev = ray_rb2event(self);

  if (ev->type == SAY_EVENT_RESIZE)
    return ray_vector2_to_rb(ev->ev.resize.size);

  return Qnil;
}


void Init_ray_event() {
  ray_cEvent = rb_define_class_under(ray_mRay, "Event", rb_cObject);

  rb_define_alloc_func(ray_cEvent, ray_event_alloc);
  rb_define_method(ray_cEvent, "type", ray_event_type, 0);
  rb_define_method(ray_cEvent, "mouse_pos", ray_event_mouse_pos, 0);
  rb_define_method(ray_cEvent, "wheel_delta", ray_event_wheel_delta, 0);
  rb_define_method(ray_cEvent, "button", ray_event_button, 0);
  rb_define_method(ray_cEvent, "text", ray_event_text, 0);
  rb_define_method(ray_cEvent, "key", ray_event_key, 0);
  rb_define_method(ray_cEvent, "native_key", ray_event_native_key, 0);
  rb_define_method(ray_cEvent, "key_mod", ray_event_key_mod, 0);
  rb_define_method(ray_cEvent, "window_size", ray_event_window_size, 0);

  /**
   * Event types
   */

  /* No event */
  rb_define_const(ray_cEvent, "TypeNone", INT2FIX(SAY_EVENT_NONE));

  /* User trying to exit */
  rb_define_const(ray_cEvent, "TypeQuit", INT2FIX(SAY_EVENT_QUIT));

  /* The window was resized */
  rb_define_const(ray_cEvent, "TypeResize", INT2FIX(SAY_EVENT_RESIZE));

  /* Mouse moved */
  rb_define_const(ray_cEvent, "TypeMouseMotion", INT2FIX(SAY_EVENT_MOUSE_MOTION));

  /* Mouse wheel moved */
  rb_define_const(ray_cEvent, "TypeWheelMotion", INT2FIX(SAY_EVENT_WHEEL_MOTION));

  /* Mouse button pressed */
  rb_define_const(ray_cEvent, "TypeButtonPress", INT2FIX(SAY_EVENT_BUTTON_PRESS));

  /* Mouse button released */
  rb_define_const(ray_cEvent, "TypeButtonRelease", INT2FIX(SAY_EVENT_BUTTON_RELEASE));

  /* Focus lost */
  rb_define_const(ray_cEvent, "TypeFocusLoss", INT2FIX(SAY_EVENT_FOCUS_LOSS));

  /* Focus gained */
  rb_define_const(ray_cEvent, "TypeFocusGain", INT2FIX(SAY_EVENT_FOCUS_GAIN));

  /* Mouse left */
  rb_define_const(ray_cEvent, "TypeMouseLeft", INT2FIX(SAY_EVENT_MOUSE_LEFT));

  /* Mouse entered */
  rb_define_const(ray_cEvent, "TypeMouseEntered", INT2FIX(SAY_EVENT_MOUSE_ENTERED));

  /* Text entered */
  rb_define_const(ray_cEvent, "TypeTextEntered", INT2FIX(SAY_EVENT_TEXT_ENTERED));

  /* A key was pressed */
  rb_define_const(ray_cEvent, "TypeKeyPress", INT2FIX(SAY_EVENT_KEY_PRESS));

  /* A key was released */
  rb_define_const(ray_cEvent, "TypeKeyRelease", INT2FIX(SAY_EVENT_KEY_RELEASE));

  /**
   * Mouse buttons
   */

  /* A button was pressed, but Ray does not know what it corresponds to */
  rb_define_const(ray_cEvent, "ButtonUnknown", INT2FIX(SAY_BUTTON_UNKNOWN));
  rb_define_const(ray_cEvent, "ButtonLeft", INT2FIX(SAY_BUTTON_LEFT));
  rb_define_const(ray_cEvent, "ButtonRight", INT2FIX(SAY_BUTTON_RIGHT));
  rb_define_const(ray_cEvent, "ButtonMiddle", INT2FIX(SAY_BUTTON_MIDDLE));

  /**
   * Keys
   */

  /* Any key Ray doesn't know about */
  rb_define_const(ray_cEvent, "KeyUnknown", INT2FIX(SAY_KEY_UNKNOWN));
  rb_define_const(ray_cEvent, "KeyA", INT2FIX(SAY_KEY_A));
  rb_define_const(ray_cEvent, "KeyB", INT2FIX(SAY_KEY_B));
  rb_define_const(ray_cEvent, "KeyC", INT2FIX(SAY_KEY_C));
  rb_define_const(ray_cEvent, "KeyD", INT2FIX(SAY_KEY_D));
  rb_define_const(ray_cEvent, "KeyE", INT2FIX(SAY_KEY_E));
  rb_define_const(ray_cEvent, "KeyF", INT2FIX(SAY_KEY_F));
  rb_define_const(ray_cEvent, "KeyG", INT2FIX(SAY_KEY_G));
  rb_define_const(ray_cEvent, "KeyH", INT2FIX(SAY_KEY_H));
  rb_define_const(ray_cEvent, "KeyI", INT2FIX(SAY_KEY_I));
  rb_define_const(ray_cEvent, "KeyJ", INT2FIX(SAY_KEY_J));
  rb_define_const(ray_cEvent, "KeyK", INT2FIX(SAY_KEY_K));
  rb_define_const(ray_cEvent, "KeyL", INT2FIX(SAY_KEY_L));
  rb_define_const(ray_cEvent, "KeyM", INT2FIX(SAY_KEY_M));
  rb_define_const(ray_cEvent, "KeyN", INT2FIX(SAY_KEY_N));
  rb_define_const(ray_cEvent, "KeyO", INT2FIX(SAY_KEY_O));
  rb_define_const(ray_cEvent, "KeyP", INT2FIX(SAY_KEY_P));
  rb_define_const(ray_cEvent, "KeyQ", INT2FIX(SAY_KEY_Q));
  rb_define_const(ray_cEvent, "KeyR", INT2FIX(SAY_KEY_R));
  rb_define_const(ray_cEvent, "KeyS", INT2FIX(SAY_KEY_S));
  rb_define_const(ray_cEvent, "KeyT", INT2FIX(SAY_KEY_T));
  rb_define_const(ray_cEvent, "KeyU", INT2FIX(SAY_KEY_U));
  rb_define_const(ray_cEvent, "KeyV", INT2FIX(SAY_KEY_V));
  rb_define_const(ray_cEvent, "KeyW", INT2FIX(SAY_KEY_W));
  rb_define_const(ray_cEvent, "KeyX", INT2FIX(SAY_KEY_X));
  rb_define_const(ray_cEvent, "KeyY", INT2FIX(SAY_KEY_Y));
  rb_define_const(ray_cEvent, "KeyZ", INT2FIX(SAY_KEY_Z));
  rb_define_const(ray_cEvent, "KeyNum0", INT2FIX(SAY_KEY_NUM_0));
  rb_define_const(ray_cEvent, "KeyNum1", INT2FIX(SAY_KEY_NUM_1));
  rb_define_const(ray_cEvent, "KeyNum2", INT2FIX(SAY_KEY_NUM_2));
  rb_define_const(ray_cEvent, "KeyNum3", INT2FIX(SAY_KEY_NUM_3));
  rb_define_const(ray_cEvent, "KeyNum4", INT2FIX(SAY_KEY_NUM_4));
  rb_define_const(ray_cEvent, "KeyNum5", INT2FIX(SAY_KEY_NUM_5));
  rb_define_const(ray_cEvent, "KeyNum6", INT2FIX(SAY_KEY_NUM_6));
  rb_define_const(ray_cEvent, "KeyNum7", INT2FIX(SAY_KEY_NUM_7));
  rb_define_const(ray_cEvent, "KeyNum8", INT2FIX(SAY_KEY_NUM_8));
  rb_define_const(ray_cEvent, "KeyNum9", INT2FIX(SAY_KEY_NUM_9));
  rb_define_const(ray_cEvent, "KeyKp0", INT2FIX(SAY_KEY_KP_0));
  rb_define_const(ray_cEvent, "KeyKp1", INT2FIX(SAY_KEY_KP_1));
  rb_define_const(ray_cEvent, "KeyKp2", INT2FIX(SAY_KEY_KP_2));
  rb_define_const(ray_cEvent, "KeyKp3", INT2FIX(SAY_KEY_KP_3));
  rb_define_const(ray_cEvent, "KeyKp4", INT2FIX(SAY_KEY_KP_4));
  rb_define_const(ray_cEvent, "KeyKp5", INT2FIX(SAY_KEY_KP_5));
  rb_define_const(ray_cEvent, "KeyKp6", INT2FIX(SAY_KEY_KP_6));
  rb_define_const(ray_cEvent, "KeyKp7", INT2FIX(SAY_KEY_KP_7));
  rb_define_const(ray_cEvent, "KeyKp8", INT2FIX(SAY_KEY_KP_8));
  rb_define_const(ray_cEvent, "KeyKp9", INT2FIX(SAY_KEY_KP_9));
  rb_define_const(ray_cEvent, "KeyEscape", INT2FIX(SAY_KEY_ESCAPE));
  rb_define_const(ray_cEvent, "KeyBackspace", INT2FIX(SAY_KEY_BACKSPACE));
  rb_define_const(ray_cEvent, "KeyTab", INT2FIX(SAY_KEY_TAB));
  rb_define_const(ray_cEvent, "KeyReturn", INT2FIX(SAY_KEY_RETURN));
  rb_define_const(ray_cEvent, "KeyPageUp", INT2FIX(SAY_KEY_PAGE_UP));
  rb_define_const(ray_cEvent, "KeyPageDown", INT2FIX(SAY_KEY_PAGE_DOWN));
  rb_define_const(ray_cEvent, "KeyEnd", INT2FIX(SAY_KEY_END));
  rb_define_const(ray_cEvent, "KeyHome", INT2FIX(SAY_KEY_HOME));
  rb_define_const(ray_cEvent, "KeyInsert", INT2FIX(SAY_KEY_INSERT));
  rb_define_const(ray_cEvent, "KeyDelete", INT2FIX(SAY_KEY_DELETE));
  rb_define_const(ray_cEvent, "KeyPause", INT2FIX(SAY_KEY_PAUSE));
  rb_define_const(ray_cEvent, "KeyLControl", INT2FIX(SAY_KEY_LCONTROL));
  rb_define_const(ray_cEvent, "KeyLShift", INT2FIX(SAY_KEY_LSHIFT));
  rb_define_const(ray_cEvent, "KeyLMeta", INT2FIX(SAY_KEY_LMETA));
  rb_define_const(ray_cEvent, "KeyLSuper", INT2FIX(SAY_KEY_LSUPER));
  rb_define_const(ray_cEvent, "KeyRControl", INT2FIX(SAY_KEY_RCONTROL));
  rb_define_const(ray_cEvent, "KeyRShift", INT2FIX(SAY_KEY_RSHIFT));
  rb_define_const(ray_cEvent, "KeyRMeta", INT2FIX(SAY_KEY_RMETA));
  rb_define_const(ray_cEvent, "KeyRSuper", INT2FIX(SAY_KEY_RSUPER));
  rb_define_const(ray_cEvent, "KeyMenu", INT2FIX(SAY_KEY_MENU));
  rb_define_const(ray_cEvent, "KeyLbracket", INT2FIX(SAY_KEY_LBRACKET));
  rb_define_const(ray_cEvent, "KeyRbracket", INT2FIX(SAY_KEY_RBRACKET));
  rb_define_const(ray_cEvent, "KeySemicolon", INT2FIX(SAY_KEY_SEMICOLON));
  rb_define_const(ray_cEvent, "KeyComma", INT2FIX(SAY_KEY_COMMA));
  rb_define_const(ray_cEvent, "KeyPeriod", INT2FIX(SAY_KEY_PERIOD));
  rb_define_const(ray_cEvent, "KeyQuote", INT2FIX(SAY_KEY_QUOTE));
  rb_define_const(ray_cEvent, "KeySlash", INT2FIX(SAY_KEY_SLASH));
  rb_define_const(ray_cEvent, "KeyBackslash", INT2FIX(SAY_KEY_BACKSLASH));
  rb_define_const(ray_cEvent, "KeyTilde", INT2FIX(SAY_KEY_TILDE));
  rb_define_const(ray_cEvent, "KeyEqual", INT2FIX(SAY_KEY_EQUAL));
  rb_define_const(ray_cEvent, "KeyMinus", INT2FIX(SAY_KEY_MINUS));
  rb_define_const(ray_cEvent, "KeyPlus", INT2FIX(SAY_KEY_PLUS));
  rb_define_const(ray_cEvent, "KeyAsterisk", INT2FIX(SAY_KEY_ASTERISK));
  rb_define_const(ray_cEvent, "KeySpace", INT2FIX(SAY_KEY_SPACE));
  rb_define_const(ray_cEvent, "KeyLeft", INT2FIX(SAY_KEY_LEFT));
  rb_define_const(ray_cEvent, "KeyRight", INT2FIX(SAY_KEY_RIGHT));
  rb_define_const(ray_cEvent, "KeyUp", INT2FIX(SAY_KEY_UP));
  rb_define_const(ray_cEvent, "KeyDown", INT2FIX(SAY_KEY_DOWN));
  rb_define_const(ray_cEvent, "KeyF1", INT2FIX(SAY_KEY_F1));
  rb_define_const(ray_cEvent, "KeyF2", INT2FIX(SAY_KEY_F2));
  rb_define_const(ray_cEvent, "KeyF3", INT2FIX(SAY_KEY_F3));
  rb_define_const(ray_cEvent, "KeyF4", INT2FIX(SAY_KEY_F4));
  rb_define_const(ray_cEvent, "KeyF5", INT2FIX(SAY_KEY_F5));
  rb_define_const(ray_cEvent, "KeyF6", INT2FIX(SAY_KEY_F6));
  rb_define_const(ray_cEvent, "KeyF7", INT2FIX(SAY_KEY_F7));
  rb_define_const(ray_cEvent, "KeyF8", INT2FIX(SAY_KEY_F8));
  rb_define_const(ray_cEvent, "KeyF9", INT2FIX(SAY_KEY_F9));
  rb_define_const(ray_cEvent, "KeyF10", INT2FIX(SAY_KEY_F10));
  rb_define_const(ray_cEvent, "KeyF11", INT2FIX(SAY_KEY_F11));
  rb_define_const(ray_cEvent, "KeyF12", INT2FIX(SAY_KEY_F12));
  rb_define_const(ray_cEvent, "KeyF13", INT2FIX(SAY_KEY_F13));
  rb_define_const(ray_cEvent, "KeyF14", INT2FIX(SAY_KEY_F14));
  rb_define_const(ray_cEvent, "KeyF15", INT2FIX(SAY_KEY_F15));

  /**
   * Key modifiers
   */

  rb_define_const(ray_cEvent, "ModMeta", INT2FIX(SAY_MOD_META));
  rb_define_const(ray_cEvent, "ModControl", INT2FIX(SAY_MOD_CONTROL));
  rb_define_const(ray_cEvent, "ModShift", INT2FIX(SAY_MOD_SHIFT));
  rb_define_const(ray_cEvent, "ModSuper", INT2FIX(SAY_MOD_SUPER));
}
