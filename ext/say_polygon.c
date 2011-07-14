#include "say.h"

static say_vector2 say_vector2_normal(say_vector2 a, say_vector2 b) {
  say_vector2 res = say_make_vector2(a.y - b.y, a.x - b.x);

  float len = sqrtf(res.x * res.x + res.y * res.y);
  res.x /= len;
  res.y /= len;

  return res;
}

static float say_vector2_dot(say_vector2 a, say_vector2 b) {
  return (a.x * b.x + a.y * b.y);
}

static void say_polygon_fill_vertices(say_polygon *polygon,
                                      say_vertex *vertices) {
  if (polygon->point_count < 3)
    return;

  size_t i = 0;

  say_vector2 center = say_make_vector2(0, 0);
  size_t r = 0, g = 0, b = 0, a = 0;
  for (size_t j = 0; j < polygon->point_count; j++) {
    center.x += polygon->points[j].pos.x;
    center.y += polygon->points[j].pos.y;

    r += polygon->points[j].col.r;
    g += polygon->points[j].col.g;
    b += polygon->points[j].col.b;
    a += polygon->points[j].col.a;
  }

  center.x /= polygon->point_count;
  center.y /= polygon->point_count;

  say_color center_col = say_make_color(r / polygon->point_count,
                                        g / polygon->point_count,
                                        b / polygon->point_count,
                                        a / polygon->point_count);

  if (polygon->filled) {
    if (polygon->point_count <= 4) {
      for (i = 0; i < polygon->point_count; i++) {
        vertices[i].pos = polygon->points[i].pos;
        vertices[i].col = polygon->points[i].col;
      }
    }
    else {
      vertices[i].pos = center;
      vertices[i].col = center_col;

      for (i = 1; i < polygon->point_count + 1; i++) {
        vertices[i].pos = polygon->points[i - 1].pos;
        vertices[i].col = polygon->points[i - 1].col;
      }

      vertices[i++] = vertices[1];
    }
  }

  if (polygon->outlined) {
    size_t first_id = i;

    for (size_t j = 0; j < polygon->point_count; j++, i += 2) {
      say_polygon_point current = polygon->points[j];

      say_polygon_point next, prev;
      if (j == polygon->point_count - 1) {
        next = polygon->points[0];
        prev = polygon->points[j - 1];
      }
      else if (j == 0) {
        next = polygon->points[j + 1];
        prev = polygon->points[polygon->point_count - 1];
      }
      else {
        next = polygon->points[j + 1];
        prev = polygon->points[j - 1];
      }

      say_vector2 first_normal = say_vector2_normal(prev.pos, current.pos);
      say_vector2 sec_normal = say_vector2_normal(current.pos, next.pos);

      say_vector2 normal = say_make_vector2(first_normal.x + sec_normal.x,
                                            first_normal.y + sec_normal.y);
      float len = 1.0 + say_vector2_dot(first_normal, sec_normal);
      normal.x /= len;
      normal.y /= len;

      say_vector2 vector = say_make_vector2(current.pos.x - center.x,
                                            current.pos.y - center.y);

      if (normal.x * vector.x <= 0)
        normal.x *= -1;

      normal.x *= polygon->outline_width;
      normal.y *= polygon->outline_width;

      vertices[i].pos = current.pos;
      vertices[i].col = current.outline_color;

      vertices[i + 1].pos = say_make_vector2(current.pos.x + normal.x,
                                             current.pos.y + normal.y);
      vertices[i + 1].col = current.outline_color;
    }

    memcpy(&(vertices[i]), &(vertices[first_id]), sizeof(say_vertex) * 2);
  }
}

static void say_polygon_draw(say_polygon *polygon, size_t first,
                             say_shader *shader) {
  if (polygon->point_count < 3)
    return;

  size_t current = first;

  if (polygon->filled) {
    if (polygon->point_count <= 4) {
      glDrawArrays(GL_TRIANGLE_FAN, current, polygon->point_count);
      current += polygon->point_count;
    }
    else {
      glDrawArrays(GL_TRIANGLE_FAN, current, polygon->point_count + 2);
      current += polygon->point_count + 2;
    }
  }

  if (polygon->outlined) {
    glDrawArrays(GL_TRIANGLE_STRIP, current, polygon->point_count * 2 + 2);
  }
}

static void say_polygon_compute_size(say_polygon *polygon) {
  size_t count = 0;

  if (polygon->filled) {
    if (polygon->point_count <= 4)
      count += polygon->point_count;
    else
      count += polygon->point_count + 2;
  }

  if (polygon->outlined)
    count += 2 + polygon->point_count * 2;

  say_drawable_set_vertex_count(polygon->drawable, count);
}

say_polygon *say_polygon_create(size_t size) {
  say_polygon *polygon = (say_polygon*)malloc(sizeof(say_polygon));

  polygon->points      = malloc(sizeof(say_polygon_point) * size);
  polygon->point_count = size;

  polygon->outline_width = 1;
  polygon->outlined = 0;
  polygon->filled   = 1;

  polygon->drawable = say_drawable_create(0);
  say_drawable_set_custom_data(polygon->drawable, polygon);
  say_drawable_set_fill_proc(polygon->drawable,
                             (say_fill_proc)say_polygon_fill_vertices);
  say_drawable_set_render_proc(polygon->drawable,
                               (say_render_proc)say_polygon_draw);

  say_polygon_compute_size(polygon);

  return polygon;
}

void say_polygon_free(say_polygon *polygon) {
  say_drawable_free(polygon->drawable);

  free(polygon->points);
  free(polygon);
}

void say_polygon_copy(say_polygon *polygon, say_polygon *other) {
  say_drawable_copy(polygon->drawable, other->drawable);
  say_drawable_set_custom_data(polygon->drawable, polygon);

  polygon->outline_width = other->outline_width;
  polygon->outlined      = other->outlined;
  polygon->filled        = other->filled;

  say_polygon_resize(polygon, say_polygon_get_size(other));
  memcpy(polygon->points, other->points, sizeof(say_polygon_point) *
         say_polygon_get_size(other));
}

say_polygon *say_polygon_triangle(say_vector2 a, say_vector2 b, say_vector2 c,
                                  say_color color) {
  say_polygon *triangle = say_polygon_create(3);

  triangle->points[0].pos = a;
  triangle->points[1].pos = b;
  triangle->points[2].pos = c;

  say_polygon_set_color(triangle, color);

  return triangle;
}

say_polygon *say_polygon_rectangle(say_vector2 pos, say_vector2 size,
                                   say_color color) {
  say_polygon *rect = say_polygon_create(4);

  rect->points[0].pos = pos;
  rect->points[1].pos = say_make_vector2(pos.x + size.x, pos.y);
  rect->points[2].pos = say_make_vector2(pos.x + size.x, pos.y + size.y);
  rect->points[3].pos = say_make_vector2(pos.x, pos.y + size.y);

  say_polygon_set_color(rect, color);

  return rect;
}

say_polygon *say_polygon_circle(say_vector2 center, float radius, say_color color) {
  static const size_t point_count = 40;

  say_polygon *circle = say_polygon_create(point_count);

  float angle_step = 2 * SAY_PI / point_count;
  float current_angle = 0;

  for (size_t i = 0; i < point_count; i++) {
    circle->points[i].pos = say_make_vector2(center.x + radius *
                                             cosf(current_angle),
                                             center.y + radius *
                                             sinf(current_angle));

    current_angle += angle_step;
  }

  say_polygon_set_color(circle, color);

  return circle;
}

say_polygon *say_polygon_ellipse(say_vector2 center, float rx, float ry,
                                 say_color color) {
  say_polygon *ellipse = say_polygon_circle(center, rx, color);

  say_drawable_set_scale(ellipse->drawable, say_make_vector2(1, ry / rx));
  say_drawable_set_origin(ellipse->drawable, center);

  return ellipse;
}

say_polygon *say_polygon_line(say_vector2 first, say_vector2 last, float width,
                              say_color color) {
  say_polygon *line = say_polygon_create(4);

  line->points[0].pos = first;
  line->points[1].pos = last;
  line->points[2].pos = say_make_vector2(last.x + width, last.y);
  line->points[3].pos = say_make_vector2(first.x + width, first.y);

  say_polygon_set_color(line, color);

  return line;
}

void say_polygon_resize(say_polygon *polygon, size_t size) {
  polygon->point_count = size;
  polygon->points = realloc(polygon->points, sizeof(say_polygon_point) * size);
  say_polygon_compute_size(polygon);
}

void say_polygon_set_color(say_polygon *polygon, say_color color) {
  for (size_t i = 0; i < say_polygon_get_size(polygon); i++)
    polygon->points[i].col = color;
  say_drawable_set_changed(polygon->drawable);
}

void say_polygon_set_outline_color(say_polygon *polygon, say_color color) {
  for (size_t i = 0; i < say_polygon_get_size(polygon); i++)
    polygon->points[i].outline_color = color;
  say_drawable_set_changed(polygon->drawable);
}

size_t say_polygon_get_size(say_polygon *polygon) {
  return polygon->point_count;
}

void say_polygon_set_pos_for(say_polygon *polygon, size_t id, say_vector2 pos) {
  if (say_vector2_eq(pos, polygon->points[id].pos))
    return;

  polygon->points[id].pos = pos;
  say_drawable_set_changed(polygon->drawable);
}

void say_polygon_set_color_for(say_polygon *polygon, size_t id, say_color col) {
  if (say_color_eq(col, polygon->points[id].col))
    return;

  polygon->points[id].col = col;
  say_drawable_set_changed(polygon->drawable);
}

void say_polygon_set_outline_for(say_polygon *polygon, size_t id, say_color col) {
  if (say_color_eq(col, polygon->points[id].outline_color))
    return;

  polygon->points[id].outline_color = col;
  say_drawable_set_changed(polygon->drawable);
}

say_vector2 say_polygon_get_pos_for(say_polygon *polygon, size_t id) {
  return polygon->points[id].pos;
}

say_color say_polygon_get_color_for(say_polygon *polygon, size_t id) {
  return polygon->points[id].col;
}

say_color say_polygon_get_outline_for(say_polygon *polygon, size_t id) {
  return polygon->points[id].outline_color;
}

void say_polygon_set_outline(say_polygon *polygon, float size) {
  if (polygon->outline_width == size)
    return;

  polygon->outline_width = size;

  if (polygon->outlined)
    say_drawable_set_changed(polygon->drawable);
}

float say_polygon_get_outline(say_polygon *polygon) {
  return polygon->outline_width;
}

uint8_t say_polygon_outlined(say_polygon *polygon) {
  return polygon->outlined;
}

uint8_t say_polygon_filled(say_polygon *polygon) {
  return polygon->filled;
}

void say_polygon_set_outlined(say_polygon *polygon, uint8_t val) {
  polygon->outlined = val;
  say_polygon_compute_size(polygon);
}

void say_polygon_set_filled(say_polygon *polygon, uint8_t val) {
  polygon->filled = val;
  say_polygon_compute_size(polygon);
}
