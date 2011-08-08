#ifndef SAY_BASIC_TYPE_H_
#define SAY_BASIC_TYPE_H_

#include "say_all.h"

/*
 * We don't want compiler to align these structures because we're going to
 * allocate vertices without using sizeof(say_vertex).
 */

typedef struct {
  float x, y;
} __attribute__((packed)) say_vector2;

typedef struct {
  float x, y, z;
} __attribute__((packed)) say_vector3;

typedef struct {
  float x, y, w, h;
} say_rect;

typedef struct {
  uint8_t r, g, b, a;
} __attribute__((packed)) say_color;

typedef struct {
  say_vector2 pos;
  say_color   col;
  say_vector2 tex;
} __attribute__((packed)) say_vertex;

#define say_make_vector2(x, y)         ((say_vector2){x, y})
#define say_make_vector3(x, y, z)      ((say_vector3){x, y, z})
#define say_make_rect(x, y, w, h)      ((say_rect){x, y, w, h})
#define say_make_color(r, g, b, a)     ((say_color){r, g, b, a})
#define say_make_vertex(pos, col, tex) ((say_vertex){pos, col, tex})

bool say_vector2_eq(say_vector2 a, say_vector2 b);
bool say_vector3_eq(say_vector3 a, say_vector3 b);
bool say_rect_eq(say_rect a, say_rect b);
bool say_color_eq(say_color a, say_color b);

typedef void (*say_destructor)(void *data);
typedef void (*say_creator)(void *data);

say_color *say_flip_color_buffer_copy(say_color *buffer, size_t width,
                                      size_t height);
void say_flip_color_buffer(say_color *buffer, size_t width, size_t height);

#endif
