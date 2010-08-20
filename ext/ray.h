#ifndef PSP
# include "ruby.h"
#else
# include <ruby/ruby.h>
#endif

#if defined(HAVE_SDL_H) || defined(RAY_USE_FRAMEWORK)
# include <SDL.h>
#else
# include <SDL/SDL.h>
#endif

#ifdef PSP
# include <pspkernel.h>
# include <pspdebug.h>
# include <pspctrl.h>
#endif

#if defined(HAVE_SDL_IMAGE)
# if defined(HAVE_SDL_IMAGE_H) || defined(RAY_USE_FRAMEWORK)
#  include <SDL_image.h>
# else
#  include <SDL/SDL_image.h>
# endif
#endif

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif

/* Classes and modules */
extern VALUE ray_mRay;

extern VALUE ray_cImage;
extern VALUE ray_cColor;
extern VALUE ray_cRect;

#ifdef PSP
extern VALUE ray_eTimeoutError;
extern VALUE ray_mWlan;
#endif

/* Macros for Ruby's C API */

#define RAY_IS_A(obj, klass) (RTEST(rb_obj_is_kind_of(obj, klass)))

#define RAY_OBJ_CLASSNAME(obj) (rb_class2name(rb_class_of(obj)))

#define RAY_SYM(string) (ID2SYM(rb_intern(string)))
#define RAY_METH(string) (rb_intern(string))

/* Data types */

typedef struct {
   VALUE self;
   SDL_Surface *surface;

   int mustFree; /* Should we call SDL_FreeSurface? */
} ray_image;

typedef struct {
   uint8_t r, g, b, a;
} ray_color;

typedef SDL_Rect ray_rect;

/* Convertion functions */

/** Converts a surface into a ruby object */
VALUE ray_create_image(SDL_Surface *surface);

/** Converts a color into a ruby object */
VALUE ray_col2rb(ray_color color);

/** Converts a rect into a ruby object */
VALUE ray_rect2rb(ray_rect rect);

/** Converts a ruby object into a color */
ray_color ray_rb2col(VALUE object);

/** Converts a ruby object into an image*/
ray_image *ray_rb2image(VALUE object);

/** Converts a ruby object into a color */
SDL_Surface *ray_rb2surface(VALUE object);

/** Converts a ruby object into a rect */
ray_rect ray_rb2rect(VALUE object);

/* Initializers */

void Init_ray_ext();
void Init_ray_image();
void Init_ray_color();
void Init_ray_rect();

#ifdef PSP
void Init_ray_psp();
#endif

#ifdef __cplusplus
#if 0
{
#endif
}
#endif
