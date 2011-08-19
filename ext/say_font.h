#ifndef SAY_FONT_H_
#define SAY_FONT_H_

#include "say_image.h"

typedef struct {
  int offset;
  say_rect bounds, sub_rect;
} say_glyph;

typedef struct {
  size_t current_width, height, y;
} say_font_row;

typedef struct {
  mo_hash  *glyphs;
  mo_array  rows;

  say_image *image;

  size_t current_height;
} say_font_page;

typedef struct {
  FT_Library library;
  FT_Face face;

  mo_hash *pages;
} say_font;

say_font *say_font_create();
void say_font_free(say_font *font);

say_font *say_font_default();

int say_font_load_from_file(say_font *font, const char *file);
int say_font_load_from_memory(say_font *font, void *buf, size_t size);

say_glyph *say_font_get_glyph(say_font *font, uint32_t codepoint, size_t size,
                              uint8_t bold);
size_t say_font_get_kerning(say_font *font, uint32_t first, uint32_t second,
                            size_t size);
size_t say_font_get_line_height(say_font *font, size_t size);
say_image *say_font_get_image(say_font *font, size_t size);

void say_font_clean_up();

#endif
