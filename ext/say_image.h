#ifndef SAY_IMAGE_H_
#define SAY_IMAGE_H_

#include "say_basic_type.h"

typedef struct say_image {
  GLuint texture;

  say_color *pixels;
  uint8_t texture_updated;

  size_t width, height;

  uint8_t smooth;
} say_image;

say_image *say_image_create();
void say_image_free(say_image *img);

size_t say_image_get_width(say_image *img);
size_t say_image_get_height(say_image *img);
say_vector2 say_image_get_size(say_image *img);

void say_image_resize(say_image *img, size_t w, size_t h);

void say_image_load_raw(say_image *img, size_t width, size_t height,
                        say_color *pixels);
int say_image_load_file(say_image *img, const char *filename);
int say_image_load_from_memory(say_image *img, size_t size, const char *buffer);
void say_image_create_with_size(say_image *img, size_t w, size_t h);

bool say_image_write_bmp(say_image *img, const char *filename);
bool say_image_write_png(say_image *img, const char *filename);
bool say_image_write_tga(say_image *img, const char *filename);
bool say_image_write(say_image *img, const char *filename);

uint8_t say_image_is_smooth(say_image *img);
void say_image_set_smooth(say_image *img, uint8_t val);

say_color say_image_get(say_image *img, size_t x, size_t y);
void say_image_set(say_image *img, size_t x, size_t y, say_color color);

say_rect say_image_get_tex_rect(say_image *img, say_rect rect);

say_color *say_image_get_buffer(say_image *img);

void say_image_bind(say_image *img);
void say_image_unbind();

void say_image_update_texture(say_image *img);

#endif
