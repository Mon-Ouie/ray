#include "say.h"

static bool say_input_is_key_in_range(say_key key) {
  return key > SAY_KEY_UNKNOWN && key < SAY_KEY_COUNT;
}

uint8_t say_input_is_holding(say_input *input, say_key key) {
  if (say_input_is_key_in_range(key))
    return input->keys[key - 1];
  else
    return 0;
}

say_vector2 say_input_get_mouse_pos(say_input *input) {
  return input->mouse_pos;
}

void say_input_reset(say_input *input) {
  memset(&input->keys, 0, sizeof(input->keys));
  input->mouse_pos = say_make_vector2(0, 0);
}

void say_input_press(say_input *input, say_key key) {
  if (say_input_is_key_in_range(key))
    input->keys[key - 1] = 1;
}

void say_input_release(say_input *input, say_key key) {
  if (say_input_is_key_in_range(key))
    input->keys[key - 1] = 0;
}

void say_input_set_mouse_pos(say_input *input, say_vector2 pos) {
  input->mouse_pos = pos;
}
