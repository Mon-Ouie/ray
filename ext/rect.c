#include "ray.h"

VALUE ray_cRect = Qnil;

void ray_free_rect(ray_rect *ptr) {
   free(ptr);
}

VALUE ray_rect2rb(ray_rect rect) {
   ray_rect *ptr = malloc(sizeof(ray_rect));
   memcpy(ptr, &rect, sizeof(ray_rect));

   VALUE obj = Data_Wrap_Struct(ray_cRect, NULL, ray_free_rect, ptr);
   return obj;
}

ray_rect ray_rb2rect(VALUE obj) {
   if (!RAY_IS_A(obj, ray_cRect)) {
      rb_raise(rb_eTypeError, "Can't convert %s into Ray::Rect",
               RAY_OBJ_CLASSNAME(obj));
   }

   ray_rect *rect;
   Data_Get_Struct(obj, ray_rect, rect);

   return *rect;
}

ray_rect ray_convert_to_rect(VALUE obj) {
   if (RAY_IS_A(obj, ray_cRect))
      return ray_rb2rect(obj);
   else if (RAY_IS_A(obj, rb_cArray))
      return ray_rb2rect(rb_apply(ray_cRect, RAY_METH("new"), obj));
   else {
      rb_raise(rb_eTypeError, "Can't convert %s into Ray::Rect",
               RAY_OBJ_CLASSNAME(obj));
   }

   /* Should never happe */
   ray_rect empty_rect = {0, 0, 0, 0};
   return empty_rect;
}

VALUE ray_alloc_rect(VALUE self) {
   ray_rect *rect = malloc(sizeof(ray_rect));
   VALUE ret = Data_Wrap_Struct(self, 0, ray_free_rect, rect);

   return ret;
}

/*
  @overload initialize(x, y)
    Creates a new rect with size set to 0, 0.

  @overload initialize(x, y, w, h)
    Creates a new rect with the specified size.

  @overload initialize(hash)
    Creates a new rect according to the keys specified in hash.

    @option hash [Integer] :x
    @option hash [Integer] :y
    @option hash [Integer, optional] :width
    @option hash [Integer, optional] :height required if width is set
*/
VALUE ray_init_rect(int argc, VALUE *argv, VALUE self) {
   VALUE x_or_hash, y, w, h;
   rb_scan_args(argc, argv, "13", &x_or_hash, &y, &w, &h);

   int type = TYPE(x_or_hash);
   if (type == T_HASH) {
      if (!NIL_P(y)) {
         rb_raise(rb_eArgError, "wrong number of arguments (%d for 1)",
                  argc);
      }

      VALUE hash = x_or_hash;

      x_or_hash = rb_hash_aref(hash, RAY_SYM("x"));
      y = rb_hash_aref(hash, RAY_SYM("y"));

      w = rb_hash_aref(hash, RAY_SYM("width"));
      h = rb_hash_aref(hash, RAY_SYM("height"));

      if (NIL_P(w)) w = rb_hash_aref(hash, RAY_SYM("w"));
      if (NIL_P(h)) h = rb_hash_aref(hash, RAY_SYM("h"));

      if (NIL_P(y)) {
         rb_raise(rb_eArgError, "Missing argument 'y'");
      }

      if (!NIL_P(w) && NIL_P(h)) {
         rb_raise(rb_eArgError, "missing argument 'height'");
      }
   }
   else {
      if (NIL_P(y)) {
         rb_raise(rb_eArgError, "wrong number of arguments (1 for 2)");
      }

      if (!NIL_P(w) && NIL_P(h)) {
         rb_raise(rb_eArgError, "wrong number of arguments (3 for 4)");
      }
   }

   ray_rect *rect;
   Data_Get_Struct(self, ray_rect, rect);

   rect->x = NUM2INT(x_or_hash);
   rect->y = NUM2INT(y);

   if (!NIL_P(w)) {
      rect->w = NUM2INT(w);
      rect->h = NUM2INT(h);
   }
   else {
      rect->w = 0;
      rect->h = 0;
   }

   return Qnil;
}

/* @return [Integer] position of the rect */
VALUE ray_rect_x(VALUE self) {
   ray_rect *rect;
   Data_Get_Struct(self, ray_rect, rect);

   return INT2FIX(rect->x);
}

/* @return [Integer] position of the rect */
VALUE ray_rect_y(VALUE self) {
   ray_rect *rect;
   Data_Get_Struct(self, ray_rect, rect);

   return INT2FIX(rect->y);
}

/* @return [Integer] size of the rect */
VALUE ray_rect_w(VALUE self) {
   ray_rect *rect;
   Data_Get_Struct(self, ray_rect, rect);

   return INT2FIX(rect->w);
}

/* @return [Integer] size of the rect */
VALUE ray_rect_h(VALUE self) {
   ray_rect *rect;
   Data_Get_Struct(self, ray_rect, rect);

   return INT2FIX(rect->h);
}

/* Sets the position of the rect */
VALUE ray_rect_set_x(VALUE self, VALUE val) {
   ray_rect *rect;
   Data_Get_Struct(self, ray_rect, rect);

   rect->x = NUM2INT(val);

   return val;
}

/* Sets the position of the rect */
VALUE ray_rect_set_y(VALUE self, VALUE val) {
   ray_rect *rect;
   Data_Get_Struct(self, ray_rect, rect);

   rect->y = NUM2INT(val);

   return val;
}

/* Sets the size of the rect */
VALUE ray_rect_set_w(VALUE self, VALUE val) {
   ray_rect *rect;
   Data_Get_Struct(self, ray_rect, rect);

   rect->w = NUM2INT(val);

   return val;
}

/* Sets the size of the rect */
VALUE ray_rect_set_h(VALUE self, VALUE val) {
   ray_rect *rect;
   Data_Get_Struct(self, ray_rect, rect);

   rect->h = NUM2INT(val);

   return val;
}

/*
  Document-class: Ray::Rect

  Rects are used to represent a part of a surface, using
  two attributes to represent its position, and two
  others to represent its size.
*/

void Init_ray_rect() {
   ray_cRect = rb_define_class_under(ray_mRay, "Rect", rb_cObject);

   rb_define_alloc_func(ray_cRect, ray_alloc_rect);
   rb_define_method(ray_cRect, "initialize", ray_init_rect, -1);

   rb_define_method(ray_cRect, "x", ray_rect_x, 0);
   rb_define_method(ray_cRect, "y", ray_rect_y, 0);
   rb_define_method(ray_cRect, "width", ray_rect_w, 0);
   rb_define_method(ray_cRect, "height", ray_rect_h, 0);

   rb_define_method(ray_cRect, "x=", ray_rect_set_x, 1);
   rb_define_method(ray_cRect, "y=", ray_rect_set_y, 1);
   rb_define_method(ray_cRect, "width=", ray_rect_set_w, 1);
   rb_define_method(ray_cRect, "height=", ray_rect_set_h, 1);
}
