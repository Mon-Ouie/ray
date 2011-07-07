#ifndef SAY_POLYGON_H_
#define SAY_POLYGON_H_

#include "say_drawable.h"

typedef struct {
  say_vector2 pos;
  say_color col;
  say_color outline_color;
} say_polygon_point;

typedef struct {
  say_drawable *drawable;

  say_polygon_point *points;
  size_t point_count;

  float outline_width;
  uint8_t outlined;
  uint8_t filled;
} say_polygon;

say_polygon *say_polygon_create(size_t size);
void say_polygon_free(say_polygon *polygon);

void say_polygon_copy(say_polygon *polygon, say_polygon *other);

say_polygon *say_polygon_triangle(say_vector2 a, say_vector2 b, say_vector2 c,
                                  say_color color);
say_polygon *say_polygon_rectangle(say_vector2 pos, say_vector2 size,
                                     say_color color);
say_polygon *say_polygon_circle(say_vector2 center, float radius, say_color color);
say_polygon *say_polygon_ellipse(say_vector2 center, float rx, float ry,
                                 say_color color);
say_polygon *say_polygon_line(say_vector2 first, say_vector2 last, float width,
                              say_color color);

void say_polygon_resize(say_polygon *polygon, size_t resize);

void say_polygon_set_color(say_polygon *polygon, say_color color);
void say_polygon_set_outline_color(say_polygon *polygon, say_color color);

size_t say_polygon_get_size(say_polygon *polygon);

void say_polygon_set_pos_for(say_polygon *polygon, size_t id, say_vector2 pos);
void say_polygon_set_color_for(say_polygon *polygon, size_t id, say_color col);
void say_polygon_set_outline_for(say_polygon *polygon, size_t id, say_color col);

say_vector2 say_polygon_get_pos_for(say_polygon *polygon, size_t id);
say_color say_polygon_get_color_for(say_polygon *polygon, size_t id);
say_color say_polygon_get_outline_for(say_polygon *polygon, size_t id);

void say_polygon_set_outline(say_polygon *polygon, float size);
float say_polygon_get_outline(say_polygon *polygon);

uint8_t say_polygon_outlined(say_polygon *polygon);
uint8_t say_polygon_filled(say_polygon *polygon);

void say_polygon_set_outlined(say_polygon *polygon, uint8_t val);
void say_polygon_set_filled(say_polygon *polygon, uint8_t val);

#endif
