#include "say.h"

static void say_sprite_compute_vertex_count(say_sprite *sprite) {
  if (!sprite->is_sheet) {
    say_drawable_set_vertex_count(sprite->drawable, 4);
  }
  else {
    say_drawable_set_vertex_count(sprite->drawable,
                                  (sprite->sheet_w) *
                                  (sprite->sheet_h) * 4);
  }
}

static void say_sprite_fill_rect(say_sprite *sprite, say_vertex *vertices,
                                 say_rect rect) {
  vertices[0].pos = say_make_vector2(0,      0);
  vertices[1].pos = say_make_vector2(rect.w, 0);
  vertices[2].pos = say_make_vector2(rect.w, rect.h);
  vertices[3].pos = say_make_vector2(0,      rect.h);

  say_rect tex_rect = say_image_get_tex_rect(sprite->image, rect);

  float first_x = sprite->flip_x ? tex_rect.x + tex_rect.w : tex_rect.x;
  float last_x  = sprite->flip_x ? tex_rect.x : tex_rect.x + tex_rect.w;

  float first_y = sprite->flip_y ? tex_rect.y + tex_rect.h : tex_rect.y;
  float last_y  = sprite->flip_y ? tex_rect.y : tex_rect.y + tex_rect.h;

  vertices[0].tex = say_make_vector2(first_x, first_y);
  vertices[1].tex = say_make_vector2(last_x,  first_y);
  vertices[2].tex = say_make_vector2(last_x,  last_y);
  vertices[3].tex = say_make_vector2(first_x, last_y);
}

static void say_sprite_fill_vertices(void *data, void *vertices_ptr) {
  say_sprite *sprite   = (say_sprite*)data;
  say_vertex *vertices = (say_vertex*)vertices_ptr;

  if (!sprite->image)
    return;

  for (size_t i = 0; i < say_drawable_get_vertex_count(sprite->drawable); i++)
    vertices[i].col = sprite->color;

  if (!sprite->is_sheet) {
    say_sprite_fill_rect(sprite, vertices, sprite->rect);
  }
  else {
    float step_x = say_sprite_get_sprite_width(sprite);
    float step_y = say_sprite_get_sprite_height(sprite);

    float tex_x = 0;
    float tex_y = 0;

    for (int y = 0; y < sprite->sheet_h; y++) {
      for (int x = 0; x < sprite->sheet_w; x++) {
        say_rect rect = say_make_rect(tex_x, tex_y, step_x, step_y);
        say_sprite_fill_rect(sprite, vertices, rect);

        vertices += 4;
        tex_x    += step_x;
      }

      tex_x  = 0;
      tex_y += step_y;
    }
  }
}

static void say_sprite_draw(void *data, size_t first, size_t index) {
  say_sprite *sprite = (say_sprite*)data;

  if (!sprite->image)
    return;

  if (sprite->is_sheet) {
    first += 4 * ((sprite->sheet_y * sprite->sheet_w) + sprite->sheet_x);
  }

  say_image_bind(sprite->image);
  glDrawArrays(GL_TRIANGLE_FAN, first, 4);
}

say_sprite *say_sprite_create() {
  say_sprite *sprite = malloc(sizeof(say_sprite));

  sprite->drawable = say_drawable_create(0);
  say_drawable_set_custom_data(sprite->drawable, sprite);
  say_drawable_set_vertex_count(sprite->drawable, 4);
  say_drawable_set_textured(sprite->drawable, 1);
  say_drawable_set_fill_proc(sprite->drawable, say_sprite_fill_vertices);
  say_drawable_set_render_proc(sprite->drawable, say_sprite_draw);

  sprite->image = NULL;

  sprite->color = say_make_color(255, 255, 255, 255);

  sprite->flip_x = 0;
  sprite->flip_y = 0;

  sprite->is_sheet = false;

  sprite->sheet_x = 0;
  sprite->sheet_y = 0;
  sprite->sheet_w = 1;
  sprite->sheet_h = 1;

  return sprite;
}

void say_sprite_free(say_sprite *sprite) {
  say_drawable_free(sprite->drawable);
  free(sprite);
}

void say_sprite_copy(say_sprite *sprite, say_sprite *orig) {
  say_drawable_copy(sprite->drawable, orig->drawable);

  sprite->image = orig->image;

  sprite->color = orig->color;
  sprite->rect  = orig->rect;

  sprite->flip_x = orig->flip_x;
  sprite->flip_y = orig->flip_y;

  sprite->is_sheet = orig->is_sheet;
  sprite->sheet_w  = orig->sheet_w;
  sprite->sheet_h  = orig->sheet_h;
  sprite->sheet_x  = orig->sheet_x;
  sprite->sheet_y  = orig->sheet_y;
}

say_image *say_sprite_get_image(say_sprite *sprite) {
  return sprite->image;
}

void say_sprite_set_image(say_sprite *sprite, say_image *img) {
  sprite->image = img;

  if (img) {
    say_sprite_set_rect(sprite, say_make_rect(0, 0, say_image_get_width(img),
                                              say_image_get_height(img)));
  }
}

say_color say_sprite_get_color(say_sprite *sprite) {
  return sprite->color;
}

void say_sprite_set_color(say_sprite *sprite, say_color color) {
  if (say_color_eq(color, sprite->color))
    return;

  sprite->color = color;
  say_drawable_set_changed(sprite->drawable);
}

say_rect say_sprite_get_rect(say_sprite *sprite) {
  if (sprite->is_sheet) {
    float w = say_sprite_get_sprite_width(sprite);
    float h = say_sprite_get_sprite_height(sprite);

    float x = w * sprite->sheet_x;
    float y = h * sprite->sheet_y;

    return say_make_rect(x, y, w, h);
  }
  else
    return sprite->rect;
}

void say_sprite_set_rect(say_sprite *sprite, say_rect rect) {
  if (say_rect_eq(sprite->rect, rect))
    return;

  sprite->rect = rect;
  say_drawable_set_changed(sprite->drawable);
}

void say_sprite_flip_x(say_sprite *sprite, uint8_t flip_x) {
  if (sprite->flip_x == flip_x)
    return;

  sprite->flip_x = flip_x;
  say_drawable_set_changed(sprite->drawable);
}

void say_sprite_flip_y(say_sprite *sprite, uint8_t flip_y) {
  if (sprite->flip_y == flip_y)
    return;

  sprite->flip_y = flip_y;
  say_drawable_set_changed(sprite->drawable);
}

uint8_t say_sprite_is_x_flipped(say_sprite *sprite) {
  return sprite->flip_x;
}

uint8_t say_sprite_is_y_flipped(say_sprite *sprite) {
  return sprite->flip_y;
}

bool say_sprite_uses_sprite_sheet(say_sprite *sprite) {
  return sprite->is_sheet;
}

void say_sprite_disable_sprite_sheet(say_sprite *sprite) {
  if (sprite->is_sheet) {
    sprite->is_sheet = false;

    say_sprite_compute_vertex_count(sprite);
    say_drawable_set_changed(sprite->drawable);
  }
}

void say_sprite_set_sheet_size(say_sprite *sprite, say_vector2 size) {
  int w = (int)(size.x);
  int h = (int)(size.y);

  if (w <= 0) w = 1;
  if (h <= 0) h = 1;

  if (sprite->sheet_w != w || sprite->sheet_h != h ||
      !sprite->is_sheet) {
    sprite->is_sheet = true;

    sprite->sheet_w = w;
    sprite->sheet_h = h;
    sprite->sheet_x = 0;
    sprite->sheet_y = 0;

    say_sprite_compute_vertex_count(sprite);
    say_drawable_set_changed(sprite->drawable);
  }
}

void say_sprite_set_sheet_pos(say_sprite *sprite, say_vector2 pos) {
  if (sprite->is_sheet) {
    sprite->sheet_x = floor(pos.x);
    sprite->sheet_y = floor(pos.y);

    sprite->sheet_x %= sprite->sheet_w;
    sprite->sheet_y %= sprite->sheet_h;
  }
}

say_vector2 say_sprite_get_sheet_pos(say_sprite *sprite) {
  return say_make_vector2(sprite->sheet_y, sprite->sheet_y);
}

say_vector2 say_sprite_get_sheet_size(say_sprite *sprite) {
  return say_make_vector2(sprite->sheet_w, sprite->sheet_h);
}

float say_sprite_get_sprite_width(say_sprite *sprite) {
  if (sprite->image && sprite->is_sheet) {
    return say_image_get_width(sprite->image) / (float)sprite->sheet_w;
  }
  else
    return 0.0;
}

float say_sprite_get_sprite_height(say_sprite *sprite) {
  if (sprite->image && sprite->is_sheet) {
    return say_image_get_height(sprite->image) / (float)sprite->sheet_h;
  }
  else
    return 0.0;
}

