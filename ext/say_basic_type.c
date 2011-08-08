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

say_color *say_flip_color_buffer_copy(say_color *buf, size_t width,
                                      size_t height) {
  size_t     line_size = sizeof(say_color) * width;
  say_color *ret       = malloc(line_size * height);

  for (size_t y = 0; y < height; y++)
    memcpy(&ret[y * width], &buf[(height - y - 1) * width], line_size);

  return ret;
}

void say_flip_color_buffer(say_color *buffer, size_t width, size_t height) {
  size_t     line_size = sizeof(say_color) * width;
  say_color *temp_line = malloc(line_size);

  for (size_t y = 0; y < height / 2.0; y++) {
    memcpy(temp_line, &buffer[y * width], line_size);
    memcpy(&buffer[y * width], &buffer[(height - y - 1) * width], line_size);
    memcpy(&buffer[(height - y - 1) * width], temp_line, line_size);
  }

  free(temp_line);
}
