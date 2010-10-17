#include "ray.h"

VALUE ray_cColor = Qnil;

ray_color ray_rb2col(VALUE object) {
   if (!RAY_IS_A(object, ray_cColor)) {
      rb_raise(rb_eTypeError, "Can't convert %s into Ray::Color",
               RAY_OBJ_CLASSNAME(object));
   }

   ray_color *ret = NULL;
   Data_Get_Struct(object, ray_color, ret);

   return *ret;
}

VALUE ray_col2rb(ray_color color) {
   VALUE ret = rb_funcall(ray_cColor, RAY_METH("new"), 4,
                          INT2FIX(color.r),
                          INT2FIX(color.g),
                          INT2FIX(color.b),
                          INT2FIX(color.a));   
   return ret;
}

void ray_free_color(ray_color *color) {
   free(color);
}

VALUE ray_alloc_color(VALUE self) {
   ray_color *ptr = malloc(sizeof(ray_color));
   VALUE ret = Data_Wrap_Struct(self, 0, ray_free_color, ptr);

   return ret;
}

/*
  @overload initialize(red, green, blue, alpha = 255)
    Creates a new color. All the parameters must be integers between
    0 and 255. Alpha is the transparency (255: opaque, 0: invisible)
*/
VALUE ray_init_color(int argc, VALUE *argv, VALUE self) {
   VALUE r, g, b, a;
   rb_scan_args(argc, argv, "31", &r, &g, &b, &a);
   if (a == Qnil) a = INT2FIX(255);

   ray_color *ret = NULL;
   Data_Get_Struct(self, ray_color, ret);

   ret->r = NUM2INT(r);
   ret->g = NUM2INT(g);
   ret->b = NUM2INT(b);
   ret->a = NUM2INT(a);

   return Qnil;
}

/* @return [Integer] red intensity */
VALUE ray_color_r(VALUE self) {
   ray_color *ret;
   Data_Get_Struct(self, ray_color, ret);

   return INT2FIX(ret->r);
}

/* @return [Integer] green intensity */
VALUE ray_color_g(VALUE self) {
   ray_color *ret;
   Data_Get_Struct(self, ray_color, ret);

   return INT2FIX(ret->g);
}

/* @return [Integer] blue intensity */
VALUE ray_color_b(VALUE self) {
   ray_color *ret;
   Data_Get_Struct(self, ray_color, ret);

   return INT2FIX(ret->b);
}

/* @return [Integer] alpha intensity */
VALUE ray_color_a(VALUE self) {
   ray_color *ret;
   Data_Get_Struct(self, ray_color, ret);

   return INT2FIX(ret->a);
}

/* Sets the red intensity */
VALUE ray_color_set_r(VALUE self, VALUE val) {
   ray_color *ret;
   Data_Get_Struct(self, ray_color, ret);

   ret->r = NUM2INT(val);
   return val;
}

/* Sets the green intensity */
VALUE ray_color_set_g(VALUE self, VALUE val) {
   ray_color *ret;
   Data_Get_Struct(self, ray_color, ret);

   ret->g = NUM2INT(val);
   return val;
}

/* Sets the blue intensity */
VALUE ray_color_set_b(VALUE self, VALUE val) {
   ray_color *ret;
   Data_Get_Struct(self, ray_color, ret);

   ret->b = NUM2INT(val);
   return val;
}

/* Sets the alpha intensity */
VALUE ray_color_set_a(VALUE self, VALUE val) {
   ray_color *ret;
   Data_Get_Struct(self, ray_color, ret);

   ret->a = NUM2INT(val);
   return val;
}

/*
  Document-class: Ray::Color

  Represents a color, which does not rely on the pixel format of any
  surfaces.
*/
void Init_ray_color() {
   ray_cColor = rb_define_class_under(ray_mRay, "Color", rb_cObject);
   
   rb_define_alloc_func(ray_cColor, ray_alloc_color);
   rb_define_method(ray_cColor, "initialize", ray_init_color, -1);
   
   rb_define_method(ray_cColor, "r", ray_color_r, 0);
   rb_define_method(ray_cColor, "g", ray_color_g, 0);
   rb_define_method(ray_cColor, "b", ray_color_b, 0);
   rb_define_method(ray_cColor, "a", ray_color_a, 0);

   rb_define_method(ray_cColor, "r=", ray_color_set_r, 1);
   rb_define_method(ray_cColor, "g=", ray_color_set_g, 1);
   rb_define_method(ray_cColor, "b=", ray_color_set_b, 1);
   rb_define_method(ray_cColor, "a=", ray_color_set_a, 1);
}
