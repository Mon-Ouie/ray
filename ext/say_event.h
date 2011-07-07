#ifndef SAY_EVENT_H_
#define SAY_EVENT_H_

#include "say_basic_type.h"


typedef enum {
  SAY_EVENT_NONE = 0,

  SAY_EVENT_QUIT,

  SAY_EVENT_RESIZE,

  SAY_EVENT_MOUSE_ENTERED,
  SAY_EVENT_MOUSE_LEFT,
  SAY_EVENT_FOCUS_GAIN,
  SAY_EVENT_FOCUS_LOSS,

  SAY_EVENT_WHEEL_MOTION,
  SAY_EVENT_MOUSE_MOTION,

  SAY_EVENT_BUTTON_PRESS,
  SAY_EVENT_BUTTON_RELEASE,

  SAY_EVENT_TEXT_ENTERED,

  SAY_EVENT_KEY_PRESS,
  SAY_EVENT_KEY_RELEASE
} say_event_type;

typedef enum {
  SAY_BUTTON_UNKNOWN = 0,

  SAY_BUTTON_LEFT,
  SAY_BUTTON_RIGHT,
  SAY_BUTTON_MIDDLE
} say_button;

typedef enum {
  SAY_KEY_UNKNOWN = 0,

  SAY_KEY_A,
  SAY_KEY_B,
  SAY_KEY_C,
  SAY_KEY_D,
  SAY_KEY_E,
  SAY_KEY_F,
  SAY_KEY_G,
  SAY_KEY_H,
  SAY_KEY_I,
  SAY_KEY_J,
  SAY_KEY_K,
  SAY_KEY_L,
  SAY_KEY_M,
  SAY_KEY_N,
  SAY_KEY_O,
  SAY_KEY_P,
  SAY_KEY_Q,
  SAY_KEY_R,
  SAY_KEY_S,
  SAY_KEY_T,
  SAY_KEY_U,
  SAY_KEY_V,
  SAY_KEY_W,
  SAY_KEY_X,
  SAY_KEY_Y,
  SAY_KEY_Z,

  SAY_KEY_NUM_0,
  SAY_KEY_NUM_1,
  SAY_KEY_NUM_2,
  SAY_KEY_NUM_3,
  SAY_KEY_NUM_4,
  SAY_KEY_NUM_5,
  SAY_KEY_NUM_6,
  SAY_KEY_NUM_7,
  SAY_KEY_NUM_8,
  SAY_KEY_NUM_9,

  SAY_KEY_KP_0,
  SAY_KEY_KP_1,
  SAY_KEY_KP_2,
  SAY_KEY_KP_3,
  SAY_KEY_KP_4,
  SAY_KEY_KP_5,
  SAY_KEY_KP_6,
  SAY_KEY_KP_7,
  SAY_KEY_KP_8,
  SAY_KEY_KP_9,

  SAY_KEY_ESCAPE,
  SAY_KEY_BACKSPACE,
  SAY_KEY_TAB,
  SAY_KEY_RETURN,
  SAY_KEY_PAGE_UP,
  SAY_KEY_PAGE_DOWN,
  SAY_KEY_END,
  SAY_KEY_HOME,
  SAY_KEY_INSERT,
  SAY_KEY_DELETE,
  SAY_KEY_PAUSE,

  SAY_KEY_LCONTROL,
  SAY_KEY_LSHIFT,
  SAY_KEY_LMETA,
  SAY_KEY_LSUPER,

  SAY_KEY_RCONTROL,
  SAY_KEY_RSHIFT,
  SAY_KEY_RMETA,
  SAY_KEY_RSUPER,

  SAY_KEY_MENU,
  SAY_KEY_LBRACKET,
  SAY_KEY_RBRACKET,
  SAY_KEY_SEMICOLON,
  SAY_KEY_COMMA,
  SAY_KEY_PERIOD,
  SAY_KEY_QUOTE,
  SAY_KEY_SLASH,
  SAY_KEY_BACKSLASH,
  SAY_KEY_TILDE,
  SAY_KEY_EQUAL,
  SAY_KEY_MINUS,
  SAY_KEY_PLUS,
  SAY_KEY_ASTERISK,
  SAY_KEY_SPACE,

  SAY_KEY_LEFT,
  SAY_KEY_RIGHT,
  SAY_KEY_UP,
  SAY_KEY_DOWN,

  SAY_KEY_F1,
  SAY_KEY_F2,
  SAY_KEY_F3,
  SAY_KEY_F4,
  SAY_KEY_F5,
  SAY_KEY_F6,
  SAY_KEY_F7,
  SAY_KEY_F8,
  SAY_KEY_F9,
  SAY_KEY_F10,
  SAY_KEY_F11,
  SAY_KEY_F12,
  SAY_KEY_F13,
  SAY_KEY_F14,
  SAY_KEY_F15,

  SAY_KEY_COUNT
} say_key;

#define SAY_MOD_META    0x1
#define SAY_MOD_CONTROL 0x2
#define SAY_MOD_SHIFT   0x4
#define SAY_MOD_SUPER   0x8

typedef struct {
  say_vector2 pos;
} say_event_mouse_motion;

typedef struct {
  say_vector2 pos;
  int delta;
} say_event_wheel_motion;

typedef struct {
  say_vector2 pos;
  say_button button;
} say_event_button;

typedef struct {
  uint32_t text;
} say_event_text;

typedef struct {
  say_key  code;
  uint8_t  mod;
  uint32_t native_code;
} say_event_key;

typedef struct {
  say_vector2 size;
} say_event_resize;

typedef struct say_event {
  say_event_type type;

  union {
    say_event_mouse_motion motion;
    say_event_wheel_motion wheel;
    say_event_button       button;
    say_event_text         text;
    say_event_key          key;
    say_event_resize       resize;
  } ev;
} say_event;

typedef struct {
  uint8_t keys[SAY_KEY_COUNT - 1]; /* Don't count SAY_KEY_UNKNOWN */
  say_vector2 mouse_pos;
} say_input;

uint8_t say_input_is_holding(say_input *input, say_key key);
say_vector2 say_input_get_mouse_pos(say_input *input);

void say_input_reset(say_input *input);

void say_input_press(say_input *input, say_key key);
void say_input_release(say_input *input, say_key key);

void say_input_set_mouse_pos(say_input *input, say_vector2 pos);

#endif
