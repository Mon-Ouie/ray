#include "ray.h"

#ifdef HAVE_SDL_TTF

enum ray_encoding {
   RAY_ENCODING_LATIN1,
   RAY_ENCODING_UTF8,
   RAY_ENCODING_UNICODE
};

VALUE ray_cFont = Qnil;

TTF_Font *ray_rb2font(VALUE object) {
   if (!RAY_IS_A(object, ray_cFont)) {
      rb_raise(rb_eTypeError, "Can't convert %s into Ray::Font",
               RAY_OBJ_CLASSNAME(object));
   }

   ray_font *ptr = NULL;
   Data_Get_Struct(object, ray_font, ptr);
  
   return ptr->font;
}

void ray_free_font(ray_font *font) {
   if (font->font) TTF_CloseFont(font->font);
   free(font);
}

VALUE ray_alloc_font(VALUE self) {
   ray_font *ptr = malloc(sizeof(ray_font));
   VALUE ret = Data_Wrap_Struct(self, 0, ray_free_font, ptr);

   ptr->font = NULL;
   return ret;
}

void ray_init_font_with_filename(VALUE self, VALUE filename, int size) {
   ray_font *font = NULL;
   Data_Get_Struct(self, ray_font, font);
  
   font->font = TTF_OpenFont(StringValuePtr(filename), size);
   if (!font->font) {
      rb_raise(rb_eRuntimeError, "Could not load the font (%s)",
               TTF_GetError());
   }
}

void ray_init_font_with_io(VALUE self, VALUE io, int size) {
   ray_font *font = NULL;
   Data_Get_Struct(self, ray_font, font);

   VALUE string = rb_funcall2(io, RAY_METH("read"), 0, NULL);
   char *content = StringValuePtr(string);

   SDL_RWops *data = SDL_RWFromMem(content, (int)RSTRING_LEN(string));
   if (!data) {
      rb_raise(rb_eRuntimeError, "Could not create image data (%s)",
               SDL_GetError());
   }

   font->font = TTF_OpenFontRW(data, 1, size);
   if (!font->font) {
      printf("Could not load the font (%s)\n", TTF_GetError());
      rb_raise(rb_eRuntimeError, "Could not load the font (%s)",
               TTF_GetError());
   }
}

/*
  Creates a new font.

  @param [String, #read] arg Filename of the font or IO object containing
                             the content of the font.
  @param [Integer] size Point size (based on 72DPI).
 */
VALUE ray_init_font(VALUE self, VALUE arg, VALUE size) {
   if (rb_respond_to(arg, RAY_METH("to_str")))
      ray_init_font_with_filename(self, rb_String(arg), NUM2INT(size));
   else if (rb_respond_to(arg, RAY_METH("read")))
      ray_init_font_with_io(self, arg, NUM2INT(size));
   else {
      rb_raise(rb_eTypeError, "Can't convert %s into String",
               RAY_OBJ_CLASSNAME(arg));
   }

   return Qnil;
}

/*
  @return [Integer] Bitmask describing the style of the font.
                    STYLE_NORMAL for a normal font.
*/
VALUE ray_font_style(VALUE self) {
   TTF_Font *font = ray_rb2font(self);
   return INT2FIX(TTF_GetFontStyle(font));
}

/* Sets the font style. */
VALUE ray_font_set_style(VALUE self, VALUE style) {
   TTF_Font *font = ray_rb2font(self);
   TTF_SetFontStyle(font, NUM2INT(style));
   return style;
}

/* @return [Integer] the height of the font */
VALUE ray_font_height(VALUE self) {
   TTF_Font *font = ray_rb2font(self);
   return INT2FIX(TTF_FontHeight(font));
}

/* @return [Integer] the recommended spacing between lines for this font */
VALUE ray_font_line_skip(VALUE self) {
   TTF_Font *font = ray_rb2font(self);
   return INT2FIX(TTF_FontLineSkip(font));
}

/*
  @overload size_of(text, encoding = nil)
    @param [Symbol, nil] encoding nil (for Latin1), :latin1, :utf8, or :unicode.
    @return [Ray::Rect] Size of the text.
*/
VALUE ray_font_size_of(int argc, VALUE *argv, VALUE self) {
   TTF_Font *font = ray_rb2font(self);

   VALUE string = Qnil, encoding = Qnil;
   rb_scan_args(argc, argv, "11", &string, &encoding);
   
   char *c_string = StringValuePtr(string);
   
   int w, h;
   
   if (NIL_P(encoding) || encoding == RAY_SYM("latin1"))
      TTF_SizeText(font, c_string, &w, &h);
   else if (encoding == RAY_SYM("utf8"))
      TTF_SizeUTF8(font, c_string, &w, &h);
   else if (encoding == RAY_SYM("unicode"))
      TTF_SizeUNICODE(font, (uint16_t*)c_string, &w, &h);
   else
      rb_raise(rb_eArgError, "Invalid encoding.");

   SDL_Rect rect = {0, 0, w, h};
   return ray_rect2rb(rect);
}

/*
  @oveload draw(string, opts = {})
    @param [String] string The string which should be drawn.
    
    @option opts [Symbol] :mode Drawing mode. :solid (fastest),
                                :shaded (requires a background set
                                with :background)
    @option opts [Symbol] :encoding :latin1, :utf8, or :unicode. Defaults
                                    to :latin1.
    @option opts [Ray::Color] :color Color to draw the text in. Defaults to
                                     white.
    @option opts [Ray::Color] :background The background color in :shaded mode.
                                          defaults to black.
    @option opts [Ray::Image] :on The image to draw on. In this case,
                                  it will directly draw instead of returning
                                  an image containing nothing but the string.
    @opions opts [Ray::Rect, Array<Integer>] :to, :at where to draw on the image.

    @return The surface it drew the string on.
 */
VALUE ray_font_draw(int argc, VALUE *argv, VALUE self) {
   VALUE string, hash;
   rb_scan_args(argc, argv, "11", &string, &hash);

   if (NIL_P(hash))
      hash = rb_hash_new();

   TTF_Font *font = ray_rb2font(self);

   char *c_string = StringValuePtr(string);

   /* solid, shaded, blended */
   VALUE drawing_mode = rb_hash_aref(hash, RAY_SYM("mode"));

   VALUE encoding = rb_hash_aref(hash, RAY_SYM("encoding"));
   int c_encoding = 0;
   
   if (NIL_P(encoding) || encoding == RAY_SYM("latin1"))
      c_encoding = RAY_ENCODING_LATIN1;
   else if (encoding == RAY_SYM("utf8"))
      c_encoding = RAY_ENCODING_UTF8;
   else if (encoding == RAY_SYM("unicode"))
      c_encoding = RAY_ENCODING_UNICODE;
   else
      rb_raise(rb_eArgError, "Invalid encoding.");

   VALUE rb_color = rb_hash_aref(hash, RAY_SYM("color"));
   ray_color c_color = {255, 255, 255, 255};
   if (!NIL_P(rb_color))
      c_color = ray_rb2col(rb_color);
   
   SDL_Color color = {c_color.r, c_color.g, c_color.b};
   
   SDL_Surface *surface = NULL;
   
   if (NIL_P(drawing_mode) || drawing_mode == RAY_SYM("solid")) {
      if (c_encoding == RAY_ENCODING_LATIN1)
         surface = TTF_RenderText_Solid(font, c_string, color);
      else if (c_encoding == RAY_ENCODING_UTF8)
         surface = TTF_RenderUTF8_Solid(font, c_string, color);
      else
         surface = TTF_RenderUNICODE_Solid(font, (uint16_t*)c_string, color);
   }
   else if (drawing_mode == RAY_SYM("shaded")) {
      VALUE rb_bg = rb_hash_aref(hash, RAY_SYM("background"));
      ray_color c_bg = {0, 0, 0, 255};
      if (!NIL_P(rb_bg))
         c_bg = ray_rb2col(rb_bg);
      SDL_Color bg = {c_bg.r, c_bg.g, c_bg.b};
      
      if (c_encoding == RAY_ENCODING_LATIN1)
         surface = TTF_RenderText_Shaded(font, c_string, color, bg);
      else if (c_encoding == RAY_ENCODING_UTF8)
         surface = TTF_RenderUTF8_Shaded(font, c_string, color, bg);
      else
         surface = TTF_RenderUNICODE_Shaded(font, (uint16_t*)c_string, color, bg);
   }
   else if (drawing_mode == RAY_SYM("blended")) {
      if (c_encoding == RAY_ENCODING_LATIN1)
         surface = TTF_RenderText_Blended(font, c_string, color);
      else if (c_encoding == RAY_ENCODING_UTF8)
         surface = TTF_RenderUTF8_Blended(font, c_string, color);
      else
         surface = TTF_RenderUNICODE_Blended(font, (uint16_t*)c_string, color);
   }
   else
      rb_raise(rb_eArgError, "Invalid drawing mode.");

   if (!surface) {
      rb_raise(rb_eRuntimeError, "Could not drawstring (%s)",
               TTF_GetError());
   }

   VALUE on = rb_hash_aref(hash, RAY_SYM("on"));
   if (NIL_P(on))
      return ray_create_gc_image(surface);

   SDL_Surface *target = ray_rb2surface(on);

   VALUE rb_rect = rb_hash_aref(hash, RAY_SYM("at"));
   if (NIL_P(rb_rect))
      rb_rect = rb_hash_aref(hash, RAY_SYM("to"));

   SDL_Rect rect;
   
   if (RTEST(rb_obj_is_kind_of(rb_rect, ray_cRect)))
      rect = ray_rb2rect(rb_rect);
   else if (RTEST(rb_obj_is_kind_of(rb_rect, rb_cArray)))
      rect = ray_rb2rect(rb_apply(ray_cRect, RAY_METH("new"), rb_rect));
   else {
      rb_raise(rb_eTypeError, "Can't convert %s into Ray::Rect",
               RAY_OBJ_CLASSNAME(rb_rect));
   }
   
   SDL_BlitSurface(surface, NULL, target, &rect);
   SDL_FreeSurface(surface);

   return on;
}

void Init_ray_font() {
   ray_cFont = rb_define_class_under(ray_mRay, "Font", rb_cObject);
   rb_define_alloc_func(ray_cFont, ray_alloc_font);
   rb_define_method(ray_cFont, "initialize", ray_init_font, 2);

   rb_define_method(ray_cFont, "style", ray_font_style, 0);
   rb_define_method(ray_cFont, "style=", ray_font_set_style, 1);
   
   rb_define_method(ray_cFont, "height", ray_font_height, 0);
   rb_define_method(ray_cFont, "line_skip", ray_font_line_skip, 0);

   rb_define_method(ray_cFont, "size_of", ray_font_size_of, -1);

   rb_define_method(ray_cFont, "draw", ray_font_draw, -1);
   
   rb_define_const(ray_cFont, "STYLE_NORMAL", INT2FIX(TTF_STYLE_NORMAL));
   rb_define_const(ray_cFont, "STYLE_ITALIC", INT2FIX(TTF_STYLE_ITALIC));
   rb_define_const(ray_cFont, "STYLE_BOLD", INT2FIX(TTF_STYLE_BOLD));
   rb_define_const(ray_cFont, "STYLE_UNDERLINE", INT2FIX(TTF_STYLE_UNDERLINE));
}

#endif
