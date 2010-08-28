#include "ray.h"

VALUE ray_cImage = Qnil;

ray_image *ray_rb2image(VALUE object) {
   if (!RAY_IS_A(object, ray_cImage)) {
      rb_raise(rb_eTypeError, "Can't convert %s into Ray::Image",
               RAY_OBJ_CLASSNAME(object));
   }

   ray_image *ptr = NULL;
   Data_Get_Struct(object, ray_image, ptr);
   
   return ptr;
}

SDL_Surface *ray_rb2surface(VALUE object) {
   return ray_rb2image(object)->surface;
}

void ray_init_image_with_hash(VALUE self, VALUE arg) {
   VALUE width = rb_hash_aref(arg, RAY_SYM("width"));
   if (NIL_P(width)) width = rb_hash_aref(arg, RAY_SYM("w"));

   VALUE height = rb_hash_aref(arg, RAY_SYM("height"));
   if (NIL_P(height)) height = rb_hash_aref(arg, RAY_SYM("h"));
   
   VALUE bitsperpixel = rb_hash_aref(arg, RAY_SYM("bits_per_pixel"));
   if (NIL_P(bitsperpixel)) bitsperpixel = rb_hash_aref(arg, RAY_SYM("bpp"));   
   if (NIL_P(bitsperpixel)) bitsperpixel = INT2FIX(32);

   uint32_t flags = 0;
   if (RTEST(rb_hash_aref(arg, RAY_SYM("sw_surface"))))
      flags = SDL_SWSURFACE;
   else
      flags = SDL_HWSURFACE;

   ray_image *image = ray_rb2image(self);
   image->surface = SDL_CreateRGBSurface(flags,
                                         NUM2INT(width),
                                         NUM2INT(height),
                                         NUM2INT(bitsperpixel),
                                         0, 0, 0, 0);

   if (!image->surface) {
      rb_raise(rb_eRuntimeError, "Could not create the image (%s)",
               SDL_GetError());
   }
}

void ray_init_image_with_filename(VALUE self, VALUE filename) {
   char *c_filename = StringValuePtr(filename);
   ray_image *image = ray_rb2image(self);

#ifdef HAVE_SDL_IMAGE
   image->surface = IMG_Load(c_filename);

   if (!image->surface) {
      rb_raise(rb_eRuntimeError, "Could not create the image (%s)",
               IMG_GetError());
   }
#else
   image->surface = SDL_LoadBMP(c_filename);
   
   if (!image->surface) {
      rb_raise(rb_eRuntimeError, "Could not create the image (%s)",
               SDL_GetError());
   }
#endif
}

void ray_init_image_with_io(VALUE self, VALUE io) {
   VALUE string = rb_funcall2(io, RAY_METH("read"), 0, NULL);
   char *content = StringValuePtr(string);

   SDL_RWops *data = SDL_RWFromMem(content, (int)RSTRING_LEN(string));
   
   if (!data) {
      rb_raise(rb_eRuntimeError, "Could not create image data (%s)",
               SDL_GetError());
   }

   ray_image *image = ray_rb2image(self);

#ifdef HAVE_SDL_IMAGE
   image->surface = IMG_Load_RW(data, 1);
   
   if (!image->surface) {
      rb_raise(rb_eRuntimeError, "Could not create the image (%s)",
               IMG_GetError());
   }
#else
   image->surface = SDL_LoadBMP_RW(data, 1);

   if (!image->surface) {
      rb_raise(rb_eRuntimeError, "Could not create the image (%s)",
               SDL_GetError());
   }
#endif
}

/*
  Creates a new image.

  @overload initialize(hash)
    @option hash [Integer] :width Width of the surface
    @option hash [Integer] :height Height of the surface
    
    @option hash [Integer] :w Alias for width
    @option hash [Integer] :h Alias for height

    @option hash [Integer] :bits_per_pixel See Ray.create_window
    @option hash [Integer] :pp Alias for bits_per_pixel

    @option hash [true, false] :hw_surface See Ray.create_window
    @option hash [true, false] :sw_surface See Ray.create_window

  @overload initialize(filename)
    Loads the image from a file.
    @param [String, #to_str] filename The name of the file to open

  @overload initialize(io)
    Loads the image friom an IO object.
    @param [IO, #read] io Object the data will be loaded from.
*/
VALUE ray_init_image(VALUE self, VALUE arg) {
   if (RAY_IS_A(arg, rb_cHash))
      ray_init_image_with_hash(self, arg);
   else if (rb_respond_to(arg, RAY_METH("to_str")))
      ray_init_image_with_filename(self, rb_String(arg));
   else if (rb_respond_to(arg, RAY_METH("read")))
      ray_init_image_with_io(self, arg);
   else {
      rb_raise(rb_eTypeError, "Can't convert %s into Hash",
               RAY_OBJ_CLASSNAME(arg));
   }

   return Qnil;
}

void ray_free_image(ray_image *ptr) {
   if (ptr->mustFree && ptr->surface) SDL_FreeSurface(ptr->surface);
   free(ptr);
}

VALUE ray_create_image(SDL_Surface *surface) {
   ray_image *ptr = malloc(sizeof(ray_image));
   VALUE ret = Data_Wrap_Struct(ray_cImage, 0, ray_free_image, ptr);

   ptr->surface = surface;
   ptr->mustFree = 0;

   return ret;
}

VALUE ray_alloc_image(VALUE self) {
   ray_image *ptr = malloc(sizeof(ray_image));
   VALUE ret = Data_Wrap_Struct(self, 0, ray_free_image, ptr);

   ptr->surface = NULL;
   ptr->mustFree = 1;

   return ret;
}

/*
  Fills the image with a given color.
  @param [Ray::Color] col The color used to fill the image.
 */
VALUE ray_image_fill(VALUE self, VALUE col) {
   ray_color rcol = ray_rb2col(col);
   
   SDL_Surface *surf = ray_rb2surface(self);
   uint32_t specific_col = SDL_MapRGBA(surf->format,
                                       rcol.r, rcol.g, rcol.b,
                                       rcol.a);
   
   SDL_FillRect(surf, NULL, specific_col);
   return self;
}

/* Updates the image. */
VALUE ray_image_flip(VALUE self) {
   SDL_Flip(ray_rb2surface(self));
   return self;
}

/*
  Blits the receiver on another image.
  
  @option hash [Ray::Rect, Array] :at Rects in which the image will be
                                      drawn. If an array is given, it
                                      passed to Ray::Rect.new. Only the
                                      position is read.
  @option hash [Ray::Rect, Array] :rect Rects that will be copied.
                                        If an array is given, it
                                        passed to Ray::Rect.new.
                                        If the size is (0, 0), it will
                                        be reset to the image's size.
  @option hash [Ray::Rect, Array] :from Alias for rect

  @option hash [Ray::Image, required] :on The image on which the receiver should
                                          be drawn.

  @option hash [Ray::Image, required] :to Alias for on.
*/
VALUE ray_image_blit(VALUE self, VALUE hash) {
   SDL_Surface *origin = ray_rb2surface(self);

   SDL_Rect from_rect = {0, 0, origin->w, origin->h};
   SDL_Rect to_rect   = {0, 0, 0, 0};
   
   VALUE rect = rb_hash_aref(hash, RAY_SYM("at"));

   if (RTEST(rb_obj_is_kind_of(rect, ray_cRect)))
      to_rect = ray_rb2rect(rect);
   else if (RTEST(rb_obj_is_kind_of(rect, rb_cArray)))
      to_rect = ray_rb2rect(rb_apply(ray_cRect, RAY_METH("new"), rect));
   else if (rect != Qnil) {
      rb_raise(rb_eTypeError, "Can't convert %s into Ray::Rect",
               RAY_OBJ_CLASSNAME(rect));
   }

   rect = Qnil;
   rect = rb_hash_aref(hash, RAY_SYM("rect"));
   if (rect == Qnil) rect = rb_hash_aref(hash, RAY_SYM("from"));

   if (RAY_IS_A(rect, ray_cRect))
      from_rect = ray_rb2rect(rect);
   else if (RAY_IS_A(rect, rb_cArray))
      from_rect = ray_rb2rect(rb_apply(ray_cRect, RAY_METH("new"), rect));
   else if (!NIL_P(rect)) {
      rb_raise(rb_eTypeError, "Can't convert %s into Ray::Rect",
               RAY_OBJ_CLASSNAME(rect));
   }

   if (from_rect.w == 0 && from_rect.h == 0) {
      from_rect.w = origin->w;
      from_rect.h = origin->h;
   }

   VALUE surf = rb_hash_aref(hash, RAY_SYM("on"));
   if (surf == Qnil) surf = rb_hash_aref(hash, RAY_SYM("to"));
   SDL_BlitSurface(origin, &from_rect,  ray_rb2surface(surf), &to_rect);

   return surf;
}

/*
  Sets the alpha transparency.
  @param [Integer, 0..255] alpha the new transparency
*/
VALUE ray_image_set_alpha(VALUE self, VALUE alpha) {
   SDL_SetAlpha(ray_rb2surface(self), SDL_SRCALPHA | SDL_RLEACCEL,
                NUM2INT(alpha));
   return alpha;
}

/*
  Returns the flags of an image (an OR'd combination of the Ray::Image::FLAG_*
  constants)
*/
VALUE ray_image_flags(VALUE self) {
   uint32_t flags = ray_rb2surface(self)->flags;
   return INT2NUM(flags);
}

/* @return [Integer] Width of the surface */
VALUE ray_image_width(VALUE self) {
   return INT2FIX(ray_rb2surface(self)->w);
}

/* @return [Integer] Height of the surface */
VALUE ray_image_height(VALUE self) {
   return INT2FIX(ray_rb2surface(self)->h);
}

/* @return [Integer] Bits per pixel */
VALUE ray_image_bpp(VALUE self) {
   return INT2FIX(ray_rb2surface(self)->format->BitsPerPixel);
}

void Init_ray_image() {
   ray_cImage = rb_define_class_under(ray_mRay, "Image", rb_cObject);
   
   rb_define_alloc_func(ray_cImage, ray_alloc_image);
   rb_define_method(ray_cImage, "initialize", ray_init_image, 1);

   rb_define_method(ray_cImage, "fill", ray_image_fill, 1);
   rb_define_method(ray_cImage, "flip", ray_image_flip, 0);

   rb_define_method(ray_cImage, "blit", ray_image_blit, 1);

   rb_define_method(ray_cImage, "alpha=", ray_image_set_alpha, 1);

   rb_define_method(ray_cImage, "flags", ray_image_flags, 0);

   rb_define_method(ray_cImage, "width", ray_image_width, 0);
   rb_define_method(ray_cImage, "height", ray_image_height, 0);
   rb_define_method(ray_cImage, "bpp", ray_image_bpp, 0);

   rb_define_const(ray_cImage, "FLAG_ANYFORMAT", INT2FIX(SDL_ANYFORMAT));
   rb_define_const(ray_cImage, "FLAG_ASYNCBLIT", INT2FIX(SDL_ASYNCBLIT));
   rb_define_const(ray_cImage, "FLAG_DOUBLEBUF", INT2FIX(SDL_DOUBLEBUF));
   rb_define_const(ray_cImage, "FLAG_HWPALETTE", INT2FIX(SDL_HWPALETTE));
   rb_define_const(ray_cImage, "FLAG_HWACCEL", INT2FIX(SDL_HWACCEL));
   rb_define_const(ray_cImage, "FLAG_HWSURFACE", INT2FIX(SDL_HWSURFACE));
   rb_define_const(ray_cImage, "FLAG_FULLSCREEN", INT2FIX(SDL_FULLSCREEN));
   rb_define_const(ray_cImage, "FLAG_OPENGL", INT2FIX(SDL_OPENGL));
   rb_define_const(ray_cImage, "FLAG_OPENGLBLIT", INT2FIX(SDL_OPENGLBLIT));
   rb_define_const(ray_cImage, "FLAG_RESIZABLE", INT2FIX(SDL_RESIZABLE));
   rb_define_const(ray_cImage, "FLAG_RLEACCEL", INT2FIX(SDL_RLEACCEL));
   rb_define_const(ray_cImage, "FLAG_SRCALPHA", INT2FIX(SDL_SRCALPHA));
   rb_define_const(ray_cImage, "FLAG_SRCCOLORKEY", INT2FIX(SDL_SRCCOLORKEY));
   rb_define_const(ray_cImage, "FLAG_PREALLOC", INT2FIX(SDL_PREALLOC));
}
