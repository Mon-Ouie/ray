#include "say.h"

bool say_vector2_eq(say_vector2 a, say_vector2 b) {
  return a.x == b.x && a.y == b.y;
}

bool say_vector3_eq(say_vector3 a, say_vector3 b) {
  return a.x == b.x && a.y == b.y && a.z == b.z;
}

bool say_rect_eq(say_rect a, say_rect b) {
  return a.x == b.x && a.y == b.y && a.w == b.w && a.h == b.h;
}

bool say_color_eq(say_color a, say_color b) {
  return a.r == b.r && a.g == b.g  && a.b == b.b && a.a == b.a;
}
