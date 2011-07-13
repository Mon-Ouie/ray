#ifndef SAY_SPRITE_H_
#define SAY_SPRITE_H_

#include "say_drawable.h"
#include "say_image.h"

typedef struct {
  say_drawable *drawable;
  say_image *image;

  say_color color;
  say_rect  rect;

  uint8_t flip_x, flip_y;

  bool is_sheet;
  int sheet_w, sheet_h;
  int sheet_x, sheet_y;
} say_sprite;

say_sprite *say_sprite_create();
void say_sprite_free(say_sprite *sprite);

void say_sprite_copy(say_sprite *sprite, say_sprite *orig);

say_image *say_sprite_get_image(say_sprite *sprite);
void say_sprite_set_image(say_sprite *sprite, say_image *img);

say_color say_sprite_get_color(say_sprite *sprite);
void say_sprite_set_color(say_sprite *sprite, say_color color);

say_rect say_sprite_get_rect(say_sprite *sprite);
void say_sprite_set_rect(say_sprite *sprite, say_rect rect);

void say_sprite_flip_x(say_sprite *sprite, uint8_t flip_x);
void say_sprite_flip_y(say_sprite *sprite, uint8_t flip_y);

uint8_t say_sprite_is_x_flipped(say_sprite *sprite);
uint8_t say_sprite_is_y_flipped(say_sprite *sprite);

bool say_sprite_uses_sprite_sheet(say_sprite *sprite);
void say_sprite_disable_sprite_sheet(say_sprite *sprite);

void say_sprite_set_sheet_size(say_sprite *sprite, say_vector2 size);
void say_sprite_set_sheet_pos(say_sprite *sprite, say_vector2 pos);

say_vector2 say_sprite_get_sheet_pos(say_sprite *sprite);
say_vector2 say_sprite_get_sheet_size(say_sprite *sprite);

float say_sprite_get_sprite_width(say_sprite *sprite);
float say_sprite_get_sprite_height(say_sprite *sprite);

#endif
