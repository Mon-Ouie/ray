#include "say.h"

static void say_text_update_rect(say_text *text) {
  if (!text->font) {
    text->rect_size.x = 0;
    text->rect_size.y = 0;

    text->rect_updated = 1;
    return;
  }

  uint8_t is_bold = (text->style & SAY_TEXT_BOLD) != 0;

  float line_height = say_font_get_line_height(text->font, text->size);
  float space_width = say_font_get_glyph(text->font, L' ', text->size,
                                         is_bold)->offset;

  float width = 0.0, current_width = 0.0, height = 0;

  uint32_t previous = 0;

  for (size_t i = 0; i < text->str_length; i++) {
    uint32_t current = text->string[i];

    if (current == L'\n') {
      height += line_height;
      if (current_width >= width)
        width = current_width;
      current_width = 0;

      previous = 0;
    }
    else if (current == L'\t') {
      current_width += space_width * 4;
      previous = 0;
    }
    else if (current == L'\v') {
      height += line_height * 4;
      if (current_width >= width)
        width = current_width;
      current_width = 0;

      previous = 0;
    }
    else if (current == L' ') {
      current_width += space_width;
      previous = 0;
    }
    else {
      say_glyph *glyph = say_font_get_glyph(text->font, current, text->size,
                                            is_bold);
      current_width += say_font_get_kerning(text->font, previous, current, text->size);
      current_width += glyph->offset;
    }
  }

  if (current_width >= width)
    width = current_width;
  height += line_height;

  if (text->style & SAY_TEXT_ITALIC)
    width += 0.208 * text->size;

  if (text->style & SAY_TEXT_UNDERLINED) {
    height += text->size * 0.1 + text->size * (is_bold ? 0.1 : 0.07);
  }

  text->rect_size.x = width;
  text->rect_size.y = height;

  text->rect_updated = 1;
}

static void say_text_compute_vertex_count(say_text *text) {
  size_t count = 0, line_count = 1;
  for (size_t i = 0; i < text->str_length; i++) {
    if (text->string[i] != L'\n' &&
        text->string[i] != L'\t' &&
        text->string[i] != L'\v' &&
        text->string[i] != L' ') {
      count += 1;
    }
    else if (text->string[i] == L'\n' || text->string[i] == L'\v') {
      line_count += 1;
    }
  }

  text->underline_vertex = count * 6;

  if (!(text->style & SAY_TEXT_UNDERLINED)) {
    line_count = 0;
  }

  /* Will use GL_TRIANGLES to draw all the rects at once */
  say_drawable_set_vertex_count(text->drawable, (count + line_count) * 6);
}

static void say_text_fill_vertices(say_text *text, say_vertex *vertices) {
  if (!text->font)
    return;

  say_image *img = say_font_get_image(text->font, text->size);

  if (!img)
    return;

  if (!text->rect_updated)
    say_text_update_rect(text);

  /* Updating the rect may cause the image to change size */
  text->last_img_size = say_image_get_size(img);

  uint8_t is_bold       = (text->style & SAY_TEXT_BOLD) != 0;
  uint8_t is_underlined = (text->style & SAY_TEXT_UNDERLINED) != 0;
  uint8_t is_italic     = (text->style & SAY_TEXT_ITALIC) != 0;

  float line_height = say_font_get_line_height(text->font, text->size);
  float space_width = say_font_get_glyph(text->font, L' ', text->size,
                                         is_bold)->offset;

  float x = 0.0, y = line_height;
  size_t ver_id = 0, under_id = text->underline_vertex;

  uint32_t previous = 0;

  say_rect under_rect = say_image_get_tex_rect(img, say_make_rect(0.5, 0.5, 0.5, 0.5));
  float underline_offset = text->size * 0.1;
  float underline_height = text->size * (is_bold ? 0.1 : 0.07);

  float italic_coeff = is_italic ? 0.208 : 0;

  for (size_t i = 0; i < text->str_length; i++) {
    uint32_t current = text->string[i];

    if ((text->string[i] == L'\n' || text->string[i] == L'\v') &&
        is_underlined) {
      float top    = y + underline_offset;
      float bottom = top + underline_height;

      vertices[under_id + 0].col = text->color;
      vertices[under_id + 0].pos = say_make_vector2(0, top);
      vertices[under_id + 0].tex = say_make_vector2(under_rect.x, under_rect.y);

      vertices[under_id + 1].col = text->color;
      vertices[under_id + 1].pos = say_make_vector2(0, bottom);
      vertices[under_id + 1].tex = say_make_vector2(under_rect.x,
                                                    under_rect.y + under_rect.h);

      vertices[under_id + 2].col = text->color;
      vertices[under_id + 2].pos = say_make_vector2(x, bottom);
      vertices[under_id + 2].tex = say_make_vector2(under_rect.x + under_rect.w,
                                                    under_rect.y + under_rect.h);

      vertices[under_id + 3].col = text->color;
      vertices[under_id + 3].pos = say_make_vector2(x, top);
      vertices[under_id + 3].tex = say_make_vector2(under_rect.x + under_rect.w,
                                                    under_rect.y);

      vertices[under_id + 4] = vertices[under_id + 0];
      vertices[under_id + 5] = vertices[under_id + 2];

      under_id += 6;
    }

    if (current == L'\n') {
      y += line_height;
      x = 0;
      previous = 0;
    }
    else if (current == L'\t') {
      x += space_width * 4;
      previous = 0;
    }
    else if (current == L'\v') {
      y += line_height * 4;
      x = 0;
      previous = 0;
    }
    else if (current == L' ') {
      x += space_width;
      previous = 0;
    }
    else { /* A character to draw */
      say_glyph *glyph = say_font_get_glyph(text->font, current, text->size,
                                            is_bold);
      x += say_font_get_kerning(text->font, previous, current, text->size);

      say_rect tex_rect = say_image_get_tex_rect(img, glyph->sub_rect);
      say_rect rect = glyph->bounds;

      float left   = rect.x;
      float right  = rect.x + rect.w;
      float top    = rect.y;
      float bottom = rect.y + rect.h;

      vertices[ver_id + 0].col = text->color;
      vertices[ver_id + 0].pos = say_make_vector2(x + left - (italic_coeff * top),
                                                  y + top);
      vertices[ver_id + 0].tex = say_make_vector2(tex_rect.x, tex_rect.y);

      vertices[ver_id + 1].col = text->color;
      vertices[ver_id + 1].pos = say_make_vector2(x + left - (italic_coeff * bottom),
                                                  y + bottom);
      vertices[ver_id + 1].tex = say_make_vector2(tex_rect.x,
                                                  tex_rect.y + tex_rect.h);

      vertices[ver_id + 2].col = text->color;
      vertices[ver_id + 2].pos = say_make_vector2(x + right - (italic_coeff * bottom),
                                                  y + bottom);
      vertices[ver_id + 2].tex = say_make_vector2(tex_rect.x + tex_rect.w,
                                                  tex_rect.y + tex_rect.h);

      vertices[ver_id + 3].col = text->color;
      vertices[ver_id + 3].pos = say_make_vector2(x + right - (italic_coeff * top),
                                                  y + top);
      vertices[ver_id + 3].tex = say_make_vector2(tex_rect.x + tex_rect.w,
                                                  tex_rect.y);

      vertices[ver_id + 4] = vertices[ver_id + 0];
      vertices[ver_id + 5] = vertices[ver_id + 2];

      ver_id += 6;

      x += glyph->offset;
    }
  }

  if (is_underlined) { /* Underline the last line */
    float top    = y + underline_offset;
    float bottom = top + underline_height;

    vertices[under_id + 0].col = text->color;
    vertices[under_id + 0].pos = say_make_vector2(0, top);
    vertices[under_id + 0].tex = say_make_vector2(under_rect.x, under_rect.y);

    vertices[under_id + 1].col = text->color;
    vertices[under_id + 1].pos = say_make_vector2(0, bottom);
    vertices[under_id + 1].tex = say_make_vector2(under_rect.x,
                                                  under_rect.y + under_rect.h);

    vertices[under_id + 2].col = text->color;
    vertices[under_id + 2].pos = say_make_vector2(x, bottom);
    vertices[under_id + 2].tex = say_make_vector2(under_rect.x + under_rect.w,
                                                  under_rect.y + under_rect.h);

    vertices[under_id + 3].col = text->color;
    vertices[under_id + 3].pos = say_make_vector2(x, top);
    vertices[under_id + 3].tex = say_make_vector2(under_rect.x + under_rect.w,
                                                  under_rect.y);

    vertices[under_id + 4] = vertices[under_id + 0];
    vertices[under_id + 5] = vertices[under_id + 2];

    under_id += 6;
  }
}

static void say_text_draw(say_text *text, size_t first, say_shader *shader) {
  if (!text->font)
    return;

  say_image *img = say_font_get_image(text->font, text->size);

  if (!img)
    return;

  say_vector2 img_size = say_image_get_size(img);
  if (img_size.x != text->last_img_size.x ||
      img_size.y != text->last_img_size.y) {
    /*
      Unfortunately, this means we'd have to draw the text a second time to
      display it correctly (but we won't let the text appear in this case).

      There should be a way to update the text whenever image size is changed.
    */
    say_drawable_set_changed(text->drawable);
  }
  else {
    say_image_bind(img);
    glDrawArrays(GL_TRIANGLES, first,
                 say_drawable_get_vertex_count(text->drawable));
  }
}

say_text *say_text_create() {
  say_text *text = malloc(sizeof(say_text));

  text->drawable = say_drawable_create(0);
  say_drawable_set_custom_data(text->drawable, text);
  say_drawable_set_textured(text->drawable, 1);
  say_drawable_set_fill_proc(text->drawable, (say_fill_proc)say_text_fill_vertices);
  say_drawable_set_render_proc(text->drawable, (say_render_proc)say_text_draw);

  text->font             = say_font_default();
  text->size             = 30;
  text->string           = NULL;
  text->str_length       = 0;
  text->style            = 0;
  text->color            = say_make_color(255, 255, 255, 255);
  text->rect_size        = say_make_vector2(0, 0);
  text->rect_updated     = 1;
  text->underline_vertex = 0;

  return text;
}

void say_text_free(say_text *text) {
  if (text->string)
    free(text->string);

  say_drawable_free(text->drawable);
  free(text);
}

void say_text_copy(say_text *text, say_text *src) {
  say_drawable_copy(text->drawable, src->drawable);

  text->font = src->font;
  text->size = src->size;

  say_text_set_string(text, src->string, src->str_length);

  text->style = src->style;

  text->color = src->color;

  text->rect_size    = src->rect_size;
  text->rect_updated = src->rect_updated;

  text->last_img_size = src->last_img_size;

  text->underline_vertex = src->underline_vertex;
}

uint32_t *say_text_get_string(say_text *text) {
  return text->string;
}

size_t say_text_get_string_length(say_text *text) {
  return text->str_length;
}

void say_text_set_string(say_text *text, uint32_t *string, size_t length) {
  if (length != text->str_length) {
    text->string = realloc(text->string, sizeof(uint32_t) * length);
    text->str_length = length;
  }

  memcpy(text->string, string, sizeof(uint32_t) * length);
  say_drawable_set_changed(text->drawable);
  text->rect_updated = 0;

  say_text_compute_vertex_count(text);
}

say_font *say_text_get_font(say_text *text) {
  return text->font;
}

void say_text_set_font(say_text *text, say_font *font) {
  text->font = font;
  say_drawable_set_changed(text->drawable);
  text->rect_updated = 0;
}

size_t say_text_get_size(say_text *text) {
  return text->size;
}

void say_text_set_size(say_text *text, size_t size) {
  text->size = size;
  say_drawable_set_changed(text->drawable);
  text->rect_updated = 0;
}

uint8_t say_text_get_style(say_text *text) {
  return text->style;
}

void say_text_set_style(say_text *text, size_t style) {
  text->style = style;
  say_drawable_set_changed(text->drawable);
  text->rect_updated = 0;
  say_text_compute_vertex_count(text);
}

say_color say_text_get_color(say_text *text) {
  return text->color;
}

void say_text_set_color(say_text *text, say_color col) {
  text->color = col;
  say_drawable_set_changed(text->drawable);
}

say_rect say_text_get_rect(say_text *text) {
  if (!text->rect_updated)
    say_text_update_rect(text);

  say_vector2 pos   = say_drawable_get_pos(text->drawable);
  say_vector2 scale = say_drawable_get_scale(text->drawable);
  say_vector2 size  = text->rect_size;

  say_rect rect = (say_rect){
    pos.x,
    pos.y,
    size.x * scale.x,
    size.y * scale.y
  };

  return rect;
}
