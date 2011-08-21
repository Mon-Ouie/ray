#include "ray.h"

VALUE ray_cPolygon = Qnil;

say_polygon *ray_rb2polygon(VALUE obj) {
  if (!RAY_IS_A(obj, rb_path2class("Ray::Polygon"))) {
    rb_raise(rb_eTypeError, "Can't convert %s into Ray::Polygon",
             RAY_OBJ_CLASSNAME(obj));
  }

  say_polygon *poly;
  Data_Get_Struct(obj, say_polygon, poly);

  return poly;
}

static
VALUE ray_polygon_alloc(VALUE self) {
  say_polygon *obj = say_polygon_create(0);
  VALUE rb = Data_Wrap_Struct(self, NULL, say_polygon_free, obj);

  say_drawable_set_shader_proc(obj->drawable, ray_drawable_shader_proc);
  say_drawable_set_other_data(obj->drawable, (void*)rb);
  rb_iv_set(rb, "@shader_attributes", Qnil);

  return rb;
}

static
VALUE ray_polygon_init_copy(VALUE self, VALUE other) {
  say_polygon_copy(ray_rb2polygon(self), ray_rb2polygon(other));
  ray_drawable_copy_attr(self, other);
  return self;
}

static
void ray_polygon_make_outline(say_polygon *poly, VALUE outline_width,
                              VALUE outline) {
  if (!NIL_P(outline_width)) {
    say_polygon_set_outlined(poly, 1);
    say_polygon_set_outline(poly, NUM2DBL(outline_width));

    if (!NIL_P(outline))
      say_polygon_set_outline_color(poly, ray_rb2col(outline));
    else
      say_polygon_set_outline_color(poly, say_make_color(255, 255, 255, 255));
  }
}

/*
 * @overload line(first, last, width = 1, color = Ray::Color.white)
 *   @param [Vector2, #to_vector2] first First point of the line
 *   @param [Vector2, #to_vector2] last Last point of the line
 *
 *   @param [Float] width Width of the line
 *
 *   @param [Ray::Color] color Color of the line
 *
 *   @return [Ray::Polygon] A line
 */
static
VALUE ray_polygon_line(int argc, VALUE *argv, VALUE self) {
  VALUE first = Qnil, last = Qnil, width = Qnil, color = Qnil;
  rb_scan_args(argc, argv, "22", &first, &last, &width, &color);

  say_polygon *line = say_polygon_line(ray_convert_to_vector2(first),
                                       ray_convert_to_vector2(last),
                                       NIL_P(width) ? 1 : NUM2DBL(width),
                                       NIL_P(color) ?
                                       say_make_color(255, 255, 255, 255) :
                                       ray_rb2col(color));
  VALUE rb = Data_Wrap_Struct(self, NULL, say_polygon_free, line);

  say_drawable_set_shader_proc(line->drawable, ray_drawable_shader_proc);
  say_drawable_set_other_data(line->drawable, (void*)rb);
  rb_iv_set(rb, "@shader_attributes", Qnil);

  return rb;
}

/*
 * @overload rectangle(rect, color = Ray::Color.white, outline_width = 0,
 *                     outline_color = Ray::Color.white)
 *   @param [Ray::Rect, #to_rect] rect Rectangle occupied by the polygon
 *   @param [Ray::Color] color Color of the rectangle
 *   @param [Float] outline_width Width of the outline
 *   @param [Ray::Color] outline_color Color of the outline
 *
 *   @return [Ray::Polygon] A rectangle
 */
static
VALUE ray_polygon_rectangle(int argc, VALUE *argv, VALUE self) {
  VALUE rb_rect = Qnil, rb_color = Qnil, outline_width = Qnil, outline = Qnil;
  rb_scan_args(argc, argv, "13", &rb_rect, &rb_color, &outline_width, &outline);

  say_rect rect   = ray_convert_to_rect(rb_rect);
  say_color color = NIL_P(rb_color) ? say_make_color(255, 255, 255, 255) :
    ray_rb2col(rb_color);

  say_polygon *poly = say_polygon_rectangle(say_make_vector2(rect.x, rect.y),
                                            say_make_vector2(rect.w, rect.h),
                                            color);
  VALUE ret = Data_Wrap_Struct(self, NULL, say_polygon_free, poly);

  ray_polygon_make_outline(poly, outline_width, outline);

  say_drawable_set_shader_proc(poly->drawable, ray_drawable_shader_proc);
  say_drawable_set_other_data(poly->drawable, (void*)ret);
  rb_iv_set(ret, "@shader_attributes", Qnil);

  return ret;
}

/*
 * @overload circle(center, radius, color = Ray::Color.white, outline_width = 0,
 *                  outline_color = Ray::Color.white)
 *
 *   @param [Ray::Vector2] center The center of the circle
 *   @param [Float] radius The radius of the circle
 *   @param [Ray::Color] color The color of the circle
 *
 *   @param outline_width (see rectangle)
 *   @param outline_color (see rectangle)
 *
 *   @return [Ray::Polygon] A circle
 */
static
VALUE ray_polygon_circle(int argc, VALUE *argv, VALUE self) {
  VALUE rb_center = Qnil, rb_radius = Qnil, rb_color = Qnil,
    rb_outline_width = Qnil, rb_outline = Qnil;

  rb_scan_args(argc, argv, "23", &rb_center, &rb_radius, &rb_color,
               &rb_outline_width, &rb_outline);

  say_color color = NIL_P(rb_color) ? say_make_color(255, 255, 255, 255) :
    ray_rb2col(rb_color);

  say_polygon *poly = say_polygon_circle(ray_convert_to_vector2(rb_center),
                                         NUM2DBL(rb_radius),
                                         color);
  VALUE ret = Data_Wrap_Struct(self, NULL, say_polygon_free, poly);

  ray_polygon_make_outline(poly, rb_outline_width, rb_outline);

  say_drawable_set_shader_proc(poly->drawable, ray_drawable_shader_proc);
  say_drawable_set_other_data(poly->drawable, (void*)ret);
  rb_iv_set(ret, "@shader_attributes", Qnil);

  return ret;
}

/*
 * @overload ellipse(center, rx, ry, color = Ray::Color.white,
 *                   outline_width = 0, outline_color = Ray::Color.white)
 *
 *   @param [Ray::Vector2] center The center of the ellipse
 *   @param [Float] rx Horizontal radius
 *   @param [Float] ry Vertical radius
 *   @param [Ray::Color] color The color of the ellipse
 *   @param outline_width (see rectangle)
 *   @param outline_color (see rectangle)
 *
 *   @return [Ray::Polygon] An ellipse
 */
static
VALUE ray_polygon_ellipse(int argc, VALUE *argv, VALUE self) {
  VALUE rb_center = Qnil, rb_rx = Qnil, rb_ry = Qnil, rb_color = Qnil,
    rb_outline_width = Qnil, rb_outline = Qnil;

  rb_scan_args(argc, argv, "33", &rb_center, &rb_rx, &rb_ry, &rb_color,
               &rb_outline_width, &rb_outline);

  say_color color = NIL_P(rb_color) ? say_make_color(255, 255, 255, 255) :
    ray_rb2col(rb_color);

  say_polygon *poly = say_polygon_ellipse(ray_convert_to_vector2(rb_center),
                                          NUM2DBL(rb_rx), NUM2DBL(rb_ry),
                                          color);

  ray_polygon_make_outline(poly, rb_outline_width, rb_outline);

  VALUE rb = Data_Wrap_Struct(self, NULL, say_polygon_free, poly);

  say_drawable_set_shader_proc(poly->drawable, ray_drawable_shader_proc);
  say_drawable_set_other_data(poly->drawable, (void*)rb);
  rb_iv_set(ret, "@shader_attributes", Qnil);

  return rb;
}

/*
  @return [Integer] The amount of points in the polygon
*/
static
VALUE ray_polygon_size(VALUE self) {
  return INT2FIX(say_polygon_get_size(ray_rb2polygon(self)));
}

/*
  @overload add_point(pos, col = Ray::Color.white, outline = Ray::Color.white)
    Adds a point to the polygon

    @param [Ray::Vector2] pos Position of the point
    @param [Ray::Color] col Color of the polygon at that point
    @param [Ray::Color] outline Color of the outline at that point
*/
static
VALUE ray_polygon_add_point(int argc, VALUE *argv, VALUE self) {
  say_polygon *poly = ray_rb2polygon(self);

  VALUE rb_pos = Qnil, rb_col = Qnil, rb_outline_col = Qnil;
  rb_scan_args(argc, argv, "12", &rb_pos, &rb_col, &rb_outline_col);

  say_vector2 pos = ray_convert_to_vector2(rb_pos);
  say_color col = NIL_P(rb_col) ? say_make_color(255, 255, 255, 255) :
    ray_rb2col(rb_col);
  say_color outline_col = NIL_P(rb_col) ? say_make_color(255, 255, 255, 255) :
    ray_rb2col(rb_outline_col);

  size_t index = say_polygon_get_size(poly);

  say_polygon_resize(poly, say_polygon_get_size(poly) + 1);
  say_polygon_set_pos_for(poly, index, pos);
  say_polygon_set_color_for(poly, index, col);
  say_polygon_set_outline_for(poly, index, outline_col);

  return self;
}

/*
  @overload resize(n)
    Changes the amount of points in the polygon.
    @param [Integer] n New size
*/
static
VALUE ray_polygon_resize(VALUE self, VALUE size) {
  say_polygon_resize(ray_rb2polygon(self), NUM2ULONG(size));
  return self;
}

/*
  @overload filled=(val)
    Enables or disable filling.
    @param [true, false] val True to enable filling (default)
*/
static
VALUE ray_polygon_set_filled(VALUE self, VALUE val) {
  say_polygon_set_filled(ray_rb2polygon(self), RTEST(val));
  return val;
}

/*
  @overload outlined=(val)
    Enables or disable outline.
    @param [true, false] val True to enable outline.
*/
static
VALUE ray_polygon_set_outlined(VALUE self, VALUE val) {
  say_polygon_set_outlined(ray_rb2polygon(self), RTEST(val));
  return val;
}

/* @return [true, false] True if filling is enabled */
static
VALUE ray_polygon_filled(VALUE self) {
  return say_polygon_filled(ray_rb2polygon(self)) ? Qtrue : Qfalse;
}

/* @return [true, false] True if outline is enabled */
static
VALUE ray_polygon_outlined(VALUE self) {
  return say_polygon_outlined(ray_rb2polygon(self)) ? Qtrue : Qfalse;
}

/* @return [Float] The width of the outline */
static
VALUE ray_polygon_outline_width(VALUE self) {
  return rb_float_new(say_polygon_get_outline(ray_rb2polygon(self)));
}

/*
  @overload outline_width=(val)
    Sets the width of the outline.
    @param [Float] val New width.
*/
static
VALUE ray_polygon_set_outline_width(VALUE self, VALUE val) {
  say_polygon_set_outline(ray_rb2polygon(self), NUM2DBL(val));
  return val;
}

/*
  @overload pos_of(id)
    @param [Integer] id Id of the point
    @return [Ray::Vector2, nil] Position of the idth point
*/
static
VALUE ray_polygon_pos_of(VALUE self, VALUE rb_id) {
  say_polygon *poly = ray_rb2polygon(self);
  size_t id = NUM2ULONG(rb_id);

  if (id >= say_polygon_get_size(poly))
    return Qnil;
  return ray_vector2_to_rb(say_polygon_get_pos_for(poly, id));
}

/*
  @overload color_of(id)
    @param [Integer] id Id of the point
    @return [Ray::Color, nil] Color of the idth point
*/
static
VALUE ray_polygon_color_of(VALUE self, VALUE rb_id) {
  say_polygon *poly = ray_rb2polygon(self);
  size_t id = NUM2ULONG(rb_id);

  if (id >= say_polygon_get_size(poly))
    return Qnil;
  return ray_col2rb(say_polygon_get_color_for(poly, id));
}

/*
  @overload outline_of(id)
    @param [Integer] id Id of the point
    @return [Ray::Color, nil] Color of the outline at the idth point
*/
static
VALUE ray_polygon_outline_of(VALUE self, VALUE rb_id) {
  say_polygon *poly = ray_rb2polygon(self);
  size_t id = NUM2ULONG(rb_id);

  if (id >= say_polygon_get_size(poly))
    return Qnil;
  return ray_col2rb(say_polygon_get_outline_for(poly, id));
}

/*
  @overload set_pos_of(id, val)
    @param [Integer] id Id of the point
    @param [Ray::Vector2] val Position of the point
*/
static
VALUE ray_polygon_set_pos_of(VALUE self, VALUE rb_id, VALUE val) {
  say_polygon *poly = ray_rb2polygon(self);
  size_t id = NUM2ULONG(rb_id);

  if (id >= say_polygon_get_size(poly)) {
    rb_raise(rb_eArgError, "trying to change point %ld, when there are %ld points",
             id, say_polygon_get_size(poly));
  }

  say_polygon_set_pos_for(poly, id, ray_convert_to_vector2(val));

  return val;
}

/*
  @overload set_color_of(id, val)
    @param [Integer] id Id of the point
    @param [Ray::Color] val Color of the point
*/
static
VALUE ray_polygon_set_color_of(VALUE self, VALUE rb_id, VALUE val) {
  say_polygon *poly = ray_rb2polygon(self);
  size_t id = NUM2ULONG(rb_id);

  if (id >= say_polygon_get_size(poly)) {
    rb_raise(rb_eArgError, "trying to change point %ld, when there are %ld points",
             id, say_polygon_get_size(poly));
  }

  say_polygon_set_color_for(poly, id, ray_rb2col(val));

  return val;
}

/*
  @overload set_outline_of(id, val)
    @param [Integer] id Id of the point
    @param [Ray::Color] val Color of the outline of the idth point
*/
static
VALUE ray_polygon_set_outline_of(VALUE self, VALUE rb_id, VALUE val) {
  say_polygon *poly = ray_rb2polygon(self);
  size_t id = NUM2ULONG(rb_id);

  if (id >= say_polygon_get_size(poly)) {
    rb_raise(rb_eArgError, "trying to change point %ld, when there are %ld points",
             id, say_polygon_get_size(poly));
  }

  say_polygon_set_outline_for(poly, id, ray_rb2col(val));

  return val;
}

/*
  @overload color=(val)
    Sets a color to all the points of the shape.
    @param [Ray::Color] val New color
*/
static
VALUE ray_polygon_set_color(VALUE self, VALUE val) {
  say_polygon_set_color(ray_rb2polygon(self), ray_rb2col(val));
  return val;
}

/*
  @overload outline=(val)
    Sets an outline color to all the points of the shape.
    @param [Ray::Color] val New color
*/
static
VALUE ray_polygon_set_outline(VALUE self, VALUE val) {
  say_polygon_set_outline_color(ray_rb2polygon(self), ray_rb2col(val));
  return val;
}

void Init_ray_polygon() {
  ray_cPolygon = rb_define_class_under(ray_mRay, "Polygon", ray_cDrawable);
  rb_define_alloc_func(ray_cPolygon, ray_polygon_alloc);
  rb_define_method(ray_cPolygon, "initialize_copy", ray_polygon_init_copy, 1);

  rb_define_singleton_method(ray_cPolygon, "line", ray_polygon_line, -1);
  rb_define_singleton_method(ray_cPolygon, "rectangle", ray_polygon_rectangle,
                             -1);
  rb_define_singleton_method(ray_cPolygon, "circle", ray_polygon_circle, -1);
  rb_define_singleton_method(ray_cPolygon, "ellipse", ray_polygon_ellipse, -1);

  rb_define_method(ray_cPolygon, "size", ray_polygon_size, 0);
  rb_define_method(ray_cPolygon, "add_point", ray_polygon_add_point, -1);
  rb_define_method(ray_cPolygon, "resize", ray_polygon_resize, 1);

  rb_define_method(ray_cPolygon, "filled=", ray_polygon_set_filled, 1);
  rb_define_method(ray_cPolygon, "outlined=", ray_polygon_set_outlined, 1);

  rb_define_method(ray_cPolygon, "filled?", ray_polygon_filled, 0);
  rb_define_method(ray_cPolygon, "outlined?", ray_polygon_outlined, 0);

  rb_define_method(ray_cPolygon, "outline_width=",
                   ray_polygon_set_outline_width, 1);
  rb_define_method(ray_cPolygon, "outline_width", ray_polygon_outline_width, 0);

  rb_define_method(ray_cPolygon, "set_pos_of", ray_polygon_set_pos_of, 2);
  rb_define_method(ray_cPolygon, "set_color_of", ray_polygon_set_color_of, 2);
  rb_define_method(ray_cPolygon, "set_outline_of", ray_polygon_set_outline_of, 2);

  rb_define_method(ray_cPolygon, "pos_of", ray_polygon_pos_of, 1);
  rb_define_method(ray_cPolygon, "color_of", ray_polygon_color_of, 1);
  rb_define_method(ray_cPolygon, "outline_of", ray_polygon_outline_of, 1);

  rb_define_method(ray_cPolygon, "color=", ray_polygon_set_color, 1);
  rb_define_method(ray_cPolygon, "outline=", ray_polygon_set_outline, 1);
}
