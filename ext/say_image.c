#include "say.h"

#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION 1
#include "stb_image_write.h"

static void say_texture_make_current(GLuint texture, int unit) {
  say_context *context = say_context_current();

  if (context->texture_unit != unit) {
    glActiveTexture(GL_TEXTURE0 + unit);
    context->texture_unit = unit;
  }

  if (context->textures[unit] != texture) {
    glBindTexture(GL_TEXTURE_2D, texture);
    context->textures[unit] = texture;
  }
}

static void say_texture_will_delete(GLuint texture) {
  mo_array *contexts = say_context_get_all();
  for (size_t i = 0; i < contexts->size; i++) {
    say_context *context = mo_array_get_as(contexts, i, say_context*);

    for (size_t i = 0; i < SAY_MAX_TEXTURE_UNIT; i++) {
      if (context->textures[i] == texture)
        context->textures[i] = 0;
    }
  }
}

static void say_image_update_buffer(say_image *img) {
  if (img->buffer_updated)
    return;

  say_texture_make_current(img->texture, 0);
  say_pixel_bus_unbind_pack();
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                img->pixels);

  img->buffer_updated  = true;
  img->texture_updated = true;
}

say_image *say_image_create() {
  say_context_ensure();

  say_image *img = (say_image*)malloc(sizeof(say_image));

  img->texture = 0;
  glGenTextures(1, &(img->texture));

  img->pixels          = NULL;
  img->texture_updated = true;
  img->buffer_updated  = true;

  img->width  = 0;
  img->height = 0;

  img->smooth = true;
  say_image_set_smooth(img, false);

  return img;
}

void say_image_free(say_image *img) {
  say_context_ensure();

  say_texture_will_delete(img->texture);
  glDeleteTextures(1, &(img->texture));

  if (img->pixels)
    free(img->pixels);

  free(img);
}

bool say_image_load_raw(say_image *img, size_t w, size_t h, say_color *pixels) {
  if (!say_image_create_with_size(img, w, h))
    return false;

  img->texture_updated = false;

  memcpy(img->pixels, pixels, sizeof(say_color) * w * h);

  /*
   * Vertical flip to fit OpenGL convention.
   */
  say_flip_color_buffer(img->pixels, w, h);

  return true;
}

bool say_image_load_flipped_raw(say_image *img, size_t w, size_t h,
                                say_color *pixels) {
  if (!say_image_create_with_size(img, w, h))
    return false;

  img->texture_updated = false;
  memcpy(img->pixels, pixels, sizeof(say_color) * w * h);
  return true;
}

bool say_image_load_file(say_image *img, const char *filename) {
  int width, height, comp = 4;

  stbi_uc *buf = stbi_load(filename, &width, &height, &comp, 4);
  if (!buf)
    return false;

  if (!say_image_load_raw(img, width, height, (say_color*)buf))
    return false;
  stbi_image_free(buf);

  return true;
}

bool say_image_load_from_memory(say_image *img, size_t size,
                                const char *buffer) {
  int width, height, comp = 4;

  stbi_uc *buf = stbi_load_from_memory((const uint8_t*)buffer, size,
                                       &width, &height, &comp, 4);
  if (!buf)
    return false;

  if (!say_image_load_raw(img, width, height, (say_color*)buf))
    return false;
  stbi_image_free(buf);

  return true;
}

bool say_image_create_with_size(say_image *img, size_t w, size_t h) {
  if (w == 0 || h == 0) {
    say_error_set("can't create empty image");
    return false;
  }

  if (img->width != w || img->height != h) {
    if (img->pixels) free(img->pixels);

    img->pixels = malloc(sizeof(say_color) * w * h);

    say_texture_make_current(img->texture, 0);
    say_pixel_bus_unbind_unpack();
    glGetError(); /* Ignore potential previous errors */
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    if (glGetError()) {
      say_error_set("could not create texture");
      return false;
    }
  }

  img->width  = w;
  img->height = h;

  img->texture_updated = true;
  img->buffer_updated  = true;

  return true;
}

static bool say_image_assert_non_empty(say_image *img) {
  if (img->width == 0 || img->height == 0) {
    say_error_set("can't save empty image");
    return false;
  }

  return true;
}

static say_color *say_image_reversed_buffer(say_image *img) {
  return say_flip_color_buffer_copy(img->pixels, img->width,
                                    img->height);
}

bool say_image_write_bmp(say_image *img, const char *filename) {
  if (!say_image_assert_non_empty(img))
    return false;

  say_image_update_buffer(img);
  say_color *buf = say_image_reversed_buffer(img);
  stbi_write_bmp(filename, img->width, img->height, 4, buf);
  free(buf);

  return true;
}

bool say_image_write_png(say_image *img, const char *filename) {
  if (!say_image_assert_non_empty(img))
    return false;

#ifdef SAY_WIN
  say_error_set("can't save image as a PNG on windows");
  return false;
#endif

  say_image_update_buffer(img);
  say_color *buf = say_image_reversed_buffer(img);
  stbi_write_png(filename, img->width, img->height, 4, buf, 0);
  free(buf);

  return true;
}

bool say_image_write_tga(say_image *img, const char *filename) {
  if (!say_image_assert_non_empty(img))
    return false;

  say_image_update_buffer(img);
  say_color *buf = say_image_reversed_buffer(img);
  stbi_write_tga(filename, img->width, img->height, 4, buf);
  free(buf);

  return true;
}

bool say_image_write(say_image *img, const char *filename) {
  size_t len = strlen(filename);

  if (len < 4)
    return say_image_write_bmp(img, filename);
  else {
    const char *ext = (filename + len - 4);
    if (strcmp(ext, ".png") == 0 || strcmp(ext, ".PNG") == 0)
      return say_image_write_png(img, filename);
    else if (strcmp(ext, ".bmp") == 0 || strcmp(ext, ".BMP") == 0)
      return say_image_write_bmp(img, filename);
    else
      say_image_write_tga(img, filename);
  }

  return true;
}

size_t say_image_get_width(say_image *img) {
  return img->width;
}

size_t say_image_get_height(say_image *img) {
  return img->height;
}

say_vector2 say_image_get_size(say_image *img) {
  return say_make_vector2(img->width, img->height);
}

bool say_image_resize(say_image *img, size_t w, size_t h) {
  say_image_update_buffer(img);

  size_t old_w = img->width, old_h = img->height;

  say_color *cpy = malloc(sizeof(say_color) * img->width * img->height);
  memcpy(cpy, img->pixels, sizeof(say_color) * img->width * img->height);

  if (!say_image_create_with_size(img, w, h))
    return false;

  img->texture_updated = false;

  size_t row_size = sizeof(say_color) * old_h;

  for (size_t y = 0; y < old_h; y++) {
    memcpy(&img->pixels[(h - y - 1) * img->width],
           &cpy[(old_h - y - 1) * old_h], row_size);
    for (size_t x = old_w; x < img->width; x++) {
      img->pixels[(h - y - 1) * img->height + x] =
        say_make_color(255, 255, 255, 0);
    }
  }

  for (size_t y = old_h; y < img->height; y++) {
    for (size_t x = 0; x < img->width; x++) {
      img->pixels[(h - y - 1) * img->height + x] =
        say_make_color(255, 255, 255, 0);
    }
  }

  free(cpy);

  return true;
}

bool say_image_is_smooth(say_image *img) {
  return img->smooth;
}

void say_image_set_smooth(say_image *img, bool val) {
  if (img->smooth != val) {
    img->smooth = val;

    say_context_ensure();
    say_texture_make_current(img->texture, 0);

    GLenum interp = val ? GL_LINEAR : GL_NEAREST;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interp);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interp);
  }
}

say_rect say_image_get_tex_rect(say_image *img, say_rect rect) {
  if (img->width == 0 || img->height == 0)
    return say_make_rect(0, 0, 0, 0);

  return say_make_rect(rect.x / img->width, 1 - (rect.y / img->height),
                       rect.w / img->width, -(rect.h / img->height));
}

say_color *say_image_get_buffer(say_image *img) {
  say_image_update_buffer(img);
  return img->pixels;
}

void say_image_mark_out_of_date(say_image *img) {
  img->buffer_updated = false;
}

say_color say_image_get(say_image *img, size_t x, size_t y) {
  say_image_update_buffer(img);
  return img->pixels[(img->height - y - 1) * img->width + x];
}

void say_image_set(say_image *img, size_t x, size_t y, say_color color) {
  say_image_update_buffer(img);

  img->pixels[(img->height - y - 1) * img->width + x] = color;
  img->texture_updated = false;
}

void say_image_bind(say_image *img) {
  say_image_bind_to(img, 0);
}

void say_image_bind_to(say_image *img, int unit) {
  say_context_ensure();
  say_texture_make_current(img->texture, unit);

  if (!img->texture_updated)
    say_image_update_texture(img);
}

void say_image_update_texture(say_image *img) {
  if (!img->pixels)
    return;

  say_texture_make_current(img->texture, 0);
  say_pixel_bus_unbind_unpack();
  glTexSubImage2D(GL_TEXTURE_2D, 0,
                  0, 0,
                  img->width, img->height,
                  GL_RGBA, GL_UNSIGNED_BYTE, img->pixels);

  img->texture_updated = true;
}

void say_image_unbind() {
  say_context_ensure();
  for (size_t i = 0; i < SAY_MAX_TEXTURE_UNIT; i++)
    say_texture_make_current(0, i);
}

void say_image_unbind_from(int unit) {
  say_context_ensure();
  say_texture_make_current(0, unit);
}

GLuint say_image_get_texture(say_image *img) {
  return img->texture;
}
