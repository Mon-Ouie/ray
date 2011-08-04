#ifndef SAY_PIXEL_BUS_H_
#define SAY_PIXEL_BUS_H_

#include "say_target.h"
#include "say_image.h"

typedef struct say_pixel_bus {
  GLuint pbo;
  size_t size;
  GLenum mode;
} say_pixel_bus;

bool say_pixel_bus_is_available();

say_pixel_bus *say_pixel_bus_create(GLenum mode);
void say_pixel_bus_free(say_pixel_bus *bus);

size_t say_pixel_bus_pull_image(say_pixel_bus *bus,
                                say_image *img, size_t offset);
size_t say_pixel_bus_pull_target(say_pixel_bus *bus,
                                 say_target *target, size_t offset,
                                 size_t x, size_t y, size_t w, size_t h);

void say_pixel_bus_push_image(say_pixel_bus *bus,
                              say_image *img, size_t offset,
                              size_t x, size_t y, size_t w, size_t h);

void say_pixel_bus_bind_pack(say_pixel_bus *bus);
void say_pixel_bus_bind_unpack(say_pixel_bus *bus);

void say_pixel_bus_unbind_pack();
void say_pixel_bus_unbind_unpack();

void say_pixel_bus_bind(say_pixel_bus *bus);
void say_pixel_bus_unbind();

void say_pixel_bus_resize(say_pixel_bus *bus, size_t new_size);
void say_pixel_bus_resize_fast(say_pixel_bus *bus, size_t new_size);

size_t say_pixel_bus_get_size(say_pixel_bus *bus);

#endif
