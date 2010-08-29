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

VALUE ray_init_image_copy(VALUE self, VALUE obj) {
   ray_image *img = ray_rb2image(self);
   
   SDL_Surface *src = ray_rb2surface(obj);
   img->surface = SDL_ConvertSurface(src, src->format, src->flags);
   if (!img->surface) {
      rb_raise(rb_eRuntimeError, "Could not create the image (%s)",
               SDL_GetError());
   }

   return self;
}

void ray_free_image(ray_image *ptr) {
   if (ptr->must_free && ptr->surface) SDL_FreeSurface(ptr->surface);
   free(ptr);
}

VALUE ray_create_image(SDL_Surface *surface) {
   ray_image *ptr = malloc(sizeof(ray_image));
   VALUE ret = Data_Wrap_Struct(ray_cImage, 0, ray_free_image, ptr);

   ptr->surface   = surface;
   ptr->must_free = 0;

   return ret;
}

VALUE ray_create_gc_image(SDL_Surface *surface) {
   ray_image *ptr = malloc(sizeof(ray_image));
   VALUE ret = Data_Wrap_Struct(ray_cImage, 0, ray_free_image, ptr);

   ptr->surface   = surface;
   ptr->must_free = 1;

   return ret;
}

VALUE ray_alloc_image(VALUE self) {
   ray_image *ptr = malloc(sizeof(ray_image));
   VALUE ret = Data_Wrap_Struct(self, 0, ray_free_image, ptr);

   ptr->surface = NULL;
   ptr->must_free = 1;

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

  @option hash [Float] :angle Rotation in degrees.
  @option hash [Float] :zoom 1.0 for the current size
*/
VALUE ray_image_blit(VALUE self, VALUE hash) {
   SDL_Surface *origin = ray_rb2surface(self);

   SDL_Rect from_rect = {0, 0, 0, 0};
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

   VALUE surf = rb_hash_aref(hash, RAY_SYM("on"));
   if (surf == Qnil) surf = rb_hash_aref(hash, RAY_SYM("to"));

#ifdef HAVE_SDL_GFX
   VALUE rb_angle = Qnil, rb_zoom = Qnil;
   double angle = 0.0, zoom = 1.0; 

   if (!NIL_P(rb_angle = rb_hash_aref(hash, RAY_SYM("angle"))))
      angle = NUM2DBL(rb_angle);

   if (!NIL_P(rb_zoom = rb_hash_aref(hash, RAY_SYM("zoom"))))
      zoom = NUM2DBL(rb_zoom);

   if (!NIL_P(rb_angle) || !NIL_P(rb_zoom)) {
      SDL_Surface *res = rotozoomSurface(origin, angle, zoom, 1);
      if (!res) {
         rb_raise(rb_eRuntimeError, "Could not create the image (%s)",
                  SDL_GetError());
      }

      if (from_rect.w == 0 && from_rect.h == 0) {
         from_rect.w = res->w;
         from_rect.h = res->h;
      }

      SDL_BlitSurface(res, &from_rect,  ray_rb2surface(surf), &to_rect);

      SDL_FreeSurface(res);
      
      return surf;
   }
#endif

   if (from_rect.w == 0 && from_rect.h == 0) {
      from_rect.w = origin->w;
      from_rect.h = origin->h;
   }

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

/* @return [true, false] true if obj's manipulates the same surface as self */
VALUE ray_image_is_equal(VALUE self, VALUE obj) {
   if (!RAY_IS_A(obj, ray_cImage))
      return Qfalse;

   SDL_Surface *first_surface = ray_rb2surface(self);
   SDL_Surface *sec_surface = ray_rb2surface(obj);

   return (first_surface == sec_surface) ? Qtrue : Qfalse;
}

VALUE ray_image_ensure_unlock(VALUE self) {
   SDL_Surface *surface = ray_rb2surface(self);
   SDL_UnlockSurface(surface);

   return self;
}

/*
  Locks the image (allow pixel-per-pixel modifications).
  Don't forget to call unlock when you're done. You can also
  pass a bock which will be called before the image gets unlocked
  automatically.
*/
VALUE ray_image_lock(VALUE self) {
   SDL_Surface *surface = ray_rb2surface(self);
   SDL_LockSurface(surface);

   if (rb_block_given_p())
      rb_ensure(rb_yield, Qnil, ray_image_ensure_unlock, self);

   return self;
}

/*
  Unlocks the image. You must call this once you are done
  modifying the image.
*/
VALUE ray_image_unlock(VALUE self) {
   return ray_image_ensure_unlock(self);
}

/*
  @return [Ray::Color, nil] Pixel at (x, y). Nil if the point is outside the
                            image.
 */
VALUE ray_image_at(VALUE self, VALUE rb_x, VALUE rb_y) {
   SDL_Surface *surface = ray_rb2surface(self);

   int x = NUM2INT(rb_x);
   int y = NUM2INT(rb_y);

   /* (w, h) is not a valid point. Surfaces use 0-based indexing. */
   if (x >= surface->w || y >= surface->h)
      return Qnil;

   int bytes = surface->format->BytesPerPixel;
   
   uint8_t *pix = (uint8_t*)surface->pixels + y * surface->pitch + x * bytes;
   
   uint32_t res;
   switch (bytes) {
      case 1:
         res = *pix;
         break;
      case 2:
         res = *(uint16_t*)pix;
         break;
      case 3:
         if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            res = pix[0] << 16 | pix[1] << 8 | pix[2];
         else
            res = pix[0] | pix[1] << 8 | pix[2] << 16;
         break;
      case 4:
         res = *(uint32_t*)pix;
         break;
      default: /* should never happen */
         res = 0;
         break;
   }

   ray_color col;
   SDL_GetRGBA(res, surface->format, &(col.r), &(col.g), &(col.b), &(col.a));
   
   return ray_col2rb(col);
}

/*
  Sets the color of the point at (x, y)
  @raise ArgumentError If (x, y) is outside the image.
*/
VALUE ray_image_set_at(VALUE self, VALUE rb_x, VALUE rb_y, VALUE rb_col) {
   SDL_Surface *surface = ray_rb2surface(self);
      
   int x = NUM2INT(rb_x);
   int y = NUM2INT(rb_y);

   if (x >= surface->w || y >= surface->h) {
      VALUE inspect = rb_inspect(self);
      rb_raise(rb_eArgError, "(%d, %d) is outside %s",
               x, y, StringValuePtr(inspect));
   }

   int bytes = surface->format->BytesPerPixel;
   
   uint8_t *pix = (uint8_t*)surface->pixels + y * surface->pitch + x * bytes;
   
   ray_color col = ray_rb2col(rb_col);

   uint32_t val = SDL_MapRGBA(surface->format, col.r, col.g, col.b, col.a);
   SDL_GetRGBA(val, surface->format, &(col.r), &(col.g), &(col.b), &(col.a));

   switch (bytes) {
      case 1:
         *pix = val;
         break;
      case 2:
         *(uint16_t*)pix = val;
         break;
      case 3:
         if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            pix[0] = (val >> 16) & 0xff;
            pix[1] = (val >> 8) & 0xff;
            pix[2] = val & 0xff;
         }
         else {
            pix[0] = val & 0xff;
            pix[1] = (val >> 8) & 0xff;
            pix[2] = (val >> 16) & 0xff;
         }
         break;
      case 4:
         *(uint32_t*)pix = val;
         break;
   }

   return rb_col;
}

#ifdef HAVE_SDL_GFX

/*
  Rotates and zoomes on the image.
  @param [Float] angle Angle in degrees
  @param [Float] zoom
  @return [SDL::Image] the modified image.
*/
VALUE ray_image_rotozoom(VALUE self, VALUE angle, VALUE zoom) {
   SDL_Surface *surface = ray_rb2surface(self);
   SDL_Surface *res = rotozoomSurface(surface, NUM2DBL(angle),
                                      NUM2DBL(zoom), 1);
   
   if (!res) {
      rb_raise(rb_eRuntimeError, "Could not create the image (%s)",
               SDL_GetError());
   }

   return ray_create_gc_image(res);
}

/*
  Rotates and zoomes on the image, but does not create a new instance
  of Ray::Image, which may be better for memory management.
*/
VALUE ray_image_rotozoom_bang(VALUE self, VALUE angle, VALUE zoom) {
   ray_image *img = ray_rb2image(self);
   SDL_Surface *res = rotozoomSurface(img->surface, NUM2DBL(angle),
                                      NUM2DBL(zoom), 1);
   
   if (!res) {
      rb_raise(rb_eRuntimeError, "Could not create the image (%s)",
               SDL_GetError());
   }

   if (img->must_free)
      SDL_FreeSurface(img->surface);

   img->surface   = res;
   img->must_free = 1;

   return self;
}

#endif

void Init_ray_image() {
   ray_cImage = rb_define_class_under(ray_mRay, "Image", rb_cObject);
   
   rb_define_alloc_func(ray_cImage, ray_alloc_image);
   rb_define_method(ray_cImage, "initialize", ray_init_image, 1);
   rb_define_method(ray_cImage, "initialize_copy", ray_init_image_copy, 1);

   rb_define_method(ray_cImage, "fill", ray_image_fill, 1);
   rb_define_method(ray_cImage, "flip", ray_image_flip, 0);

   rb_define_method(ray_cImage, "blit", ray_image_blit, 1);

   rb_define_method(ray_cImage, "alpha=", ray_image_set_alpha, 1);

   rb_define_method(ray_cImage, "flags", ray_image_flags, 0);

   rb_define_method(ray_cImage, "width", ray_image_width, 0);
   rb_define_method(ray_cImage, "height", ray_image_height, 0);
   rb_define_method(ray_cImage, "bpp", ray_image_bpp, 0);

   rb_define_method(ray_cImage, "==", ray_image_is_equal, 1);
   
   rb_define_method(ray_cImage, "lock", ray_image_lock, 0);
   rb_define_method(ray_cImage, "unlock", ray_image_unlock, 0);
   rb_define_method(ray_cImage, "[]", ray_image_at, 2);
   rb_define_method(ray_cImage, "[]=", ray_image_set_at, 3);

#ifdef HAVE_SDL_GFX
   rb_define_method(ray_cImage, "rotozoom", ray_image_rotozoom, 2);
   rb_define_method(ray_cImage, "rotozoom!", ray_image_rotozoom_bang, 2);
#endif

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
