#include "say.h"

static void say_page_init(say_font_page *page) {
  page->glyphs = mo_hash_create(sizeof(uint32_t), sizeof(say_glyph));
  page->glyphs->hash_of = mo_hash_of_u32;
  page->glyphs->key_cmp = mo_hash_u32_cmp;

  mo_array_init(&page->rows, sizeof(say_font_row));

  page->current_height = 2;

  page->image = say_image_create();
  say_image_set_smooth(page->image, 1);
  say_image_create_with_size(page->image, 128, 128);

  for (int y = 0; y < 128; y++) {
    for (int x = 0; x < 128; x++) {
      say_image_set(page->image, x, y, say_make_color(255, 255, 255, 0));
    }
  }

  for (int y = 0; y < 2; y++) {
    for (int x = 0; x < 2; x++) {
      say_image_set(page->image, x, y, say_make_color(255, 255, 255, 255));
    }
  }
}

static void say_page_free(say_font_page *page) {
  say_image_free(page->image);

  mo_array_release(&page->rows);
  mo_hash_free(page->glyphs);
}

static say_rect say_page_find_rect(say_font_page *page, size_t width, size_t height) {
  say_font_row *found_row = NULL;

  float best_ratio = 0;

  say_font_row *end = mo_array_end(&page->rows);
  for (say_font_row *row = mo_array_at(&page->rows, 0);
       row && row < end;
       mo_array_next(&page->rows, (void**)&row)) {
    float ratio = height / (float)row->height;

    if (ratio < 0.7 || ratio > 1.0 || ratio < best_ratio)
      continue;

    if (width > say_image_get_width(page->image) - row->current_width)
      continue;

    found_row  = row;
    best_ratio = ratio;
  }

  if (!found_row) {
    int row_height = height + height / 10;

    while (page->current_height + row_height >= say_image_get_height(page->image)) {
      say_vector2 size = say_image_get_size(page->image);
      say_image_resize(page->image, size.x * 2, size.y * 2);
    }

    say_font_row row;

    row.current_width = 0;
    row.y             = page->current_height;
    row.height        = row_height;

    mo_array_push(&page->rows, &row);
    found_row = mo_array_at(&page->rows, page->rows.size - 1);

    page->current_height += row_height;
  }

  say_rect rect = say_make_rect(found_row->current_width, found_row->y,
                                width, height);
  found_row->current_width += width;

  return rect;
}

static size_t say_font_get_size(say_font *font) {
  return font->face->size->metrics.x_ppem;
}

static int say_font_set_size(say_font *font, size_t size) {
  if (say_font_get_size(font) != size) {
    int err = FT_Set_Pixel_Sizes(font->face, 0, size);
    if (err) {
      say_error_set("could not set font size");
      return 0;
    }
  }

  int err = FT_Select_Charmap(font->face, FT_ENCODING_UNICODE);
  if (err) {
    say_error_set("could not select unicode charmap");
    return 0;
  }

  return 1;
}

static say_glyph *say_font_load_glyph(say_font *font, say_font_page *page,
                                      uint32_t codepoint, uint8_t bold,
                                      size_t size) {
  uint32_t bold_codepoint = ((bold ? 1 : 0) << 31) | codepoint;

  say_glyph tmp;
  mo_hash_set(page->glyphs, &bold_codepoint, &tmp);

  say_glyph *glyph = mo_hash_get(page->glyphs, &bold_codepoint);

  glyph->offset   = 0;
  glyph->bounds   = say_make_rect(2, 0, 2, 2);
  glyph->sub_rect = say_make_rect(2, 0, 2, 2);

  if (!(font->face && say_font_set_size(font, size)))
    return glyph;

  if (FT_Load_Char(font->face, codepoint, FT_LOAD_TARGET_NORMAL) != 0)
    return glyph;

  FT_Glyph ft_glyph;
  if (FT_Get_Glyph(font->face->glyph, &ft_glyph) != 0)
    return glyph;

  FT_Pos weight = 1 << 6;
  uint8_t outline = ft_glyph->format == FT_GLYPH_FORMAT_OUTLINE;

  if (bold && outline) {
    FT_OutlineGlyph outline_glyph = (FT_OutlineGlyph)ft_glyph;
    FT_Outline_Embolden(&outline_glyph->outline, weight);
  }

  FT_Glyph_To_Bitmap(&ft_glyph, FT_RENDER_MODE_NORMAL, 0, 1);
  FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)ft_glyph;
  FT_Bitmap *bitmap = &bitmap_glyph->bitmap;

  if (bold && !outline) {
    FT_Bitmap_Embolden(font->library, bitmap, weight, weight);
  }

  glyph->offset = ft_glyph->advance.x >> 16;
  if (bold)
    glyph->offset += weight >> 6;

  int width  = bitmap->width;
  int height = bitmap->rows;

  if (width > 0 && height > 0) {
    static const int padding = 1;
    glyph->sub_rect = say_page_find_rect(page,
                                         width  + (2 * padding),
                                         height + (2 * padding));

    glyph->bounds.x = +bitmap_glyph->left - padding;
    glyph->bounds.y = -bitmap_glyph->top - padding;
    glyph->bounds.w = width   + (2 * padding);
    glyph->bounds.h = height  + (2 * padding);

    say_rect actual_rect = glyph->sub_rect;
    actual_rect.x += padding;
    actual_rect.y += padding;
    actual_rect.w -= 2 * padding;
    actual_rect.h -= 2 * padding;

    uint8_t *pixels = bitmap->buffer;

    if (bitmap->pixel_mode == FT_PIXEL_MODE_MONO) {
      for (int y = actual_rect.y; y < actual_rect.y + actual_rect.h; y++) {
        for (int x = actual_rect.x; x < actual_rect.x + actual_rect.w; x++) {
          int pixel_x = x - actual_rect.x;
          uint8_t alpha = ((pixels[pixel_x / 8]) &
                           (1 << (7 - (pixel_x % 8)))) ? 255 : 0;
          say_image_set(page->image, x, y,
                        say_make_color(255, 255, 255, alpha));
        }

        pixels += bitmap->pitch;
      }
    }
    else {
      for (int y = actual_rect.y; y < actual_rect.y + actual_rect.h;
           y++) {
        for (int x = actual_rect.x; x < actual_rect.x + actual_rect.w;
             x++) {
          int pixel_x = x - actual_rect.x;
          say_image_set(page->image, x, y,
                        say_make_color(255, 255, 255, pixels[pixel_x]));
        }

        pixels += bitmap->pitch;
      }
    }
  }

  FT_Done_Glyph(ft_glyph);

  return glyph;
}

say_font *say_font_create() {
  say_font *font = malloc(sizeof(say_font));

  int err = FT_Init_FreeType(&font->library);
  if (err) {
    free(font);

    say_error_set("could not initialize freetype library");
    return NULL;
  }

  font->face = NULL;

  font->pages = mo_hash_create(sizeof(size_t), sizeof(say_font_page));
  font->pages->release = (say_destructor)say_page_free;
  font->pages->hash_of = mo_hash_of_size;
  font->pages->key_cmp = mo_hash_size_cmp;

  return font;
}

static say_font *say_default_font = NULL;
#include "say_arial.h"

say_font *say_font_default() {
  if (!say_default_font) {
    say_default_font = say_font_create();
    say_font_load_from_memory(say_default_font, say_arial_content,
                              sizeof(say_arial_content));
  }

  return say_default_font;
}

void say_font_free(say_font *font) {
  if (font->face)
    FT_Done_Face(font->face);

  if (font->library)
    FT_Done_FreeType(font->library);

  mo_hash_free(font->pages);
  free(font);
}

int say_font_load_from_file(say_font *font, const char *file) {
  int err = FT_New_Face(font->library, file, 0, &font->face);
  if (err) {
    say_error_set("could not create face");
    return 0;
  }

  return 1;
}

int say_font_load_from_memory(say_font *font, void *buf, size_t size) {
  int err = FT_New_Memory_Face(font->library, buf, size, 0, &font->face);
  if (err) {
    say_error_set("could not create face");
    return 0;
  }

  err = FT_Select_Charmap(font->face, FT_ENCODING_UNICODE);
  if (err) {
    say_error_set("could not select unicode charmap");
    return 0;
  }

  return 1;
}

say_font_page *say_font_get_page(say_font *font, size_t size) {
  say_font_page *page = NULL;
  if ((page = mo_hash_get(font->pages, &size)))
    return page;
  else {
    say_font_page page;
    say_page_init(&page);

    mo_hash_set(font->pages, &size, &page);

    return mo_hash_get(font->pages, &size);
  }
}

say_glyph *say_font_get_glyph(say_font *font, uint32_t codepoint, size_t size, uint8_t bold) {
  say_font_page *page = say_font_get_page(font, size);
  uint32_t bold_codepoint = ((bold ? 1 : 0) << 31) | codepoint;

  say_glyph *glyph = NULL;
  if ((glyph = mo_hash_get(page->glyphs, &bold_codepoint)))
    return glyph;
  else {
    return say_font_load_glyph(font, page, codepoint, bold, size);
  }
}

size_t say_font_get_kerning(say_font *font, uint32_t first, uint32_t second,
                            size_t size) {
  if (first == 0 || second == 0)
    return 0;

  if (font->face && FT_HAS_KERNING(font->face) &&
      say_font_set_size(font, size)) {
    size_t first_index = FT_Get_Char_Index(font->face, first);
    size_t sec_index   = FT_Get_Char_Index(font->face, second);

    FT_Vector kerning;
    FT_Get_Kerning(font->face, first_index, sec_index, FT_KERNING_DEFAULT,
                   &kerning);

    return kerning.x >> 6;
  }

  return 0;
}

size_t say_font_get_line_height(say_font *font, size_t size) {
  if (font->face && say_font_set_size(font, size))
    return font->face->size->metrics.height >> 6;

  return 0;
}

say_image *say_font_get_image(say_font *font, size_t size) {
  say_font_page *page = say_font_get_page(font, size);
  return page->image;
}

void say_font_clean_up() {
  if (say_default_font)
    say_font_free(say_default_font);
  say_default_font = NULL;
}
