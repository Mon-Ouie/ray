#include "ray.h"

uint8_t ray_byte_clamp(int color) {
  if (color > 255)
    return 255;
  else if (color < 0)
    return 0;
  else
    return color;
}
