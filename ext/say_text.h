#ifndef SAY_TEXT_H_
#define SAY_TEXT_H_

#include "say_font.h"
#include "say_drawable.h"

#define SAY_TEXT_NORMAL     0x0
#define SAY_TEXT_BOLD       0x1
#define SAY_TEXT_ITALIC     0x2
#define SAY_TEXT_UNDERLINED 0x4

typedef struct {
  say_drawable *drawable;

  say_font *font;
  size_t size;

  uint32_t *string;
  size_t str_length;

  uint8_t style;

  say_color color;

  say_vector2 rect_size;
  uint8_t rect_updated;

  say_vector2 last_img_size;

  size_t underline_vertex;
} say_text;

say_text *say_text_create();
void say_text_free(say_text *text);

uint32_t *say_text_get_string(say_text *text);
size_t say_text_get_string_length(say_text *text);
void say_text_set_string(say_text *text, uint32_t *string, size_t length);

say_font *say_text_get_font(say_text *text);
void say_text_set_font(say_text *text, say_font *font);

size_t say_text_get_size(say_text *text);
void say_text_set_size(say_text *text, size_t size);

uint8_t say_text_get_style(say_text *text);
void say_text_set_style(say_text *text, size_t style);

say_color say_text_get_color(say_text *text);
void say_text_set_color(say_text *text, say_color col);

say_rect say_text_get_rect(say_text *text);

#endif /* SAY_TEXT_H_ */
