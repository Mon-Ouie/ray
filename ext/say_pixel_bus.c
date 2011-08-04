#include "say.h"

static say_context *say_pack_pbo_last_context = NULL;
static GLuint       say_pack_pbo              = 0;

static void say_pack_pbo_make_current(GLuint pbo) {
  say_context *context = say_context_current();
  if (say_pack_pbo_last_context != context ||
      pbo != say_pack_pbo) {
    say_pack_pbo              = pbo;
    say_pack_pbo_last_context = context;

    glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, pbo);
  }
}

static say_context *say_unpack_pbo_last_context = NULL;
static GLuint       say_unpack_pbo              = 0;

static void say_unpack_pbo_make_current(GLuint pbo) {
  say_context *context = say_context_current();
  if (say_unpack_pbo_last_context != context ||
      pbo != say_unpack_pbo) {
    say_unpack_pbo              = pbo;
    say_unpack_pbo_last_context = context;

    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pbo);
  }
}

static void say_pbo_will_delete(GLuint pbo) {
  if (say_pack_pbo   == pbo) say_pack_pbo   = 0;
  if (say_unpack_pbo == pbo) say_unpack_pbo = 0;
}

bool say_pixel_bus_is_available() {
  say_context_ensure();
  return __GLEW_ARB_pixel_buffer_object || __GLEW_EXT_pixel_buffer_object;
}

say_pixel_bus *say_pixel_bus_create(GLenum mode) {
  say_context_ensure();

  say_pixel_bus *bus = malloc(sizeof(say_pixel_bus));

  glGenBuffersARB(1, &bus->pbo);

  bus->size = 1024;
  bus->mode = mode;

  say_pixel_bus_bind_pack(bus);
  glBufferDataARB(GL_PIXEL_PACK_BUFFER_ARB, 1024 * sizeof(say_color), NULL,
                  mode);

  return bus;
}

void say_pixel_bus_free(say_pixel_bus *bus) {
  say_context_ensure();
  say_pbo_will_delete(bus->pbo);
  glDeleteBuffersARB(1, &bus->pbo);
  free(bus);
}

size_t say_pixel_bus_pull_image(say_pixel_bus *bus,
                                say_image *img, size_t offset) {
  say_image_bind(img);
  say_pixel_bus_bind_pack(bus);

  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                (void*)(offset * sizeof(say_color)));
  return offset + say_image_get_width(img) * say_image_get_height(img);
}

size_t say_pixel_bus_pull_target(say_pixel_bus *bus,
                                 say_target *target, size_t offset,
                                 size_t x, size_t y, size_t w, size_t h) {
  if (!say_target_make_current(target))
    return offset + w * h;

  say_pixel_bus_bind_pack(bus);

  glReadPixels(x, target->size.y - y - h, w, h, GL_RGBA,
               GL_UNSIGNED_BYTE, (void*)(offset * sizeof(say_color)));
  return offset + w * h;
}

void say_pixel_bus_push_image(say_pixel_bus *bus,
                                say_image *img, size_t offset,
                                size_t x, size_t y, size_t w, size_t h) {
  say_image_bind(img);
  say_pixel_bus_bind_unpack(bus);

  glTexSubImage2D(GL_TEXTURE_2D, 0,
                  x, y, w, h,
                  GL_RGBA, GL_UNSIGNED_BYTE,
                  (void*)(offset * sizeof(say_color)));
  say_image_mark_out_of_date(img);
}

void say_pixel_bus_bind_pack(say_pixel_bus *bus) {
  say_context_ensure();
  say_pack_pbo_make_current(bus->pbo);
}

void say_pixel_bus_bind_unpack(say_pixel_bus *bus) {
  say_context_ensure();
  say_unpack_pbo_make_current(bus->pbo);
}

void say_pixel_bus_unbind_pack() {
  if (say_pixel_bus_is_available()) say_pack_pbo_make_current(0);
}

void say_pixel_bus_unbind_unpack() {
  if (say_pixel_bus_is_available()) say_unpack_pbo_make_current(0);
}

void say_pixel_bus_bind(say_pixel_bus *bus) {
  say_pixel_bus_bind_pack(bus);
  say_pixel_bus_bind_unpack(bus);
}

void say_pixel_bus_unbind() {
  say_pixel_bus_unbind_pack();
  say_pixel_bus_unbind_unpack();
}

void say_pixel_bus_resize(say_pixel_bus *bus, size_t new_size) {
  /* This is slow, so better not go through this for no reason. */
  if (bus->size == new_size) return;

  say_pixel_bus_bind_pack(bus);

  void *buffer = malloc(sizeof(say_color) * new_size);
  glGetBufferSubDataARB(GL_PIXEL_PACK_BUFFER_ARB, 0, new_size, buffer);

  glBufferDataARB(GL_PIXEL_PACK_BUFFER_ARB, new_size * sizeof(say_color),
                  buffer, bus->mode);

  free(buffer);

  bus->size = new_size;
}

void say_pixel_bus_resize_fast(say_pixel_bus *bus, size_t new_size) {
  if (bus->size == new_size) return;

  say_pixel_bus_bind_pack(bus);
  glBufferDataARB(GL_PIXEL_PACK_BUFFER_ARB, new_size * sizeof(say_color), NULL,
                  bus->mode);

  bus->size = new_size;
}

size_t say_pixel_bus_get_size(say_pixel_bus *bus) {
  return bus->size;
}
