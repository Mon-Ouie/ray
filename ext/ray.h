#ifndef RAY_H_
#define RAY_H_

#ifndef PSP
# include "ruby.h"
#else
# include <ruby/ruby.h>
#endif

#if defined(HAVE_SDL_H)
# include <SDL.h>
#else
# include <SDL/SDL.h>
#endif

#ifdef PSP
# include <pspkernel.h>
# include <pspdebug.h>
# include <pspctrl.h>
#endif

#if defined(HAVE_SDL_IMAGE_H)
# if !defined(HAVE_SDL_IMAGE)
#  define HAVE_SDL_IMAGE
# endif
# include <SDL_image.h>
#else
# if defined(HAVE_SDL_SDL_IMAGE_H)
#  if !defined(HAVE_SDL_IMAGE)
#   define HAVE_SDL_IMAGE
#  endif
#  include <SDL/SDL_image.h>
# endif
#endif

#if defined(HAVE_SDL_ROTOZOOM_H)
# if !defined(HAVE_SDL_GFX)
#  define HAVE_SDL_GFX
# endif
# include <SDL_gfxPrimitives.h>
# include <SDL_rotozoom.h>
# include <SDL_imageFilter.h>
#else
# if defined(HAVE_SDL_SDL_ROTOZOOM_H)
#  if !defined(HAVE_SDL_GFX)
#   define HAVE_SDL_GFX
#  endif
#  include <SDL/SDL_gfxPrimitives.h>
#  include <SDL/SDL_rotozoom.h>
#  include <SDL/SDL_imageFilter.h>
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
extern VALUE ray_cEvent;

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
   int width;
   int height;

   int bpp;

   uint32_t flags;
} ray_video_mode;

typedef struct {
   SDL_Surface *surface;
   int must_free; /* Should we call SDL_FreeSurface? */
} ray_image;

typedef struct {
   uint8_t r, g, b, a;
} ray_color;

typedef SDL_Rect ray_rect;

typedef struct {
   SDL_Joystick *joystick;
} ray_joystick;

/* Convertion functions */

/** Converts a surface into a ruby object (won't free it) */
VALUE ray_create_image(SDL_Surface *surface);

/** Converts a surface into a ruby object (will free it) */
VALUE ray_create_gc_image(SDL_Surface *surface);

/** Converts a color into a ruby object */
VALUE ray_col2rb(ray_color color);

/** Converts a rect into a ruby object */
VALUE ray_rect2rb(ray_rect rect);

/** Converts a ruby object into a color */
ray_color ray_rb2col(VALUE object);

/** Parses a hash to create a video mode */
ray_video_mode ray_parse_video_mode(VALUE hash);

/** Converts a ruby object into an image*/
ray_image *ray_rb2image(VALUE object);

/** Converts a ruby object into a color */
SDL_Surface *ray_rb2surface(VALUE object);

/** Converts a ruby object into a rect */
ray_rect ray_rb2rect(VALUE object);

/** Converts a ruby object into an event */
SDL_Event *ray_rb2event(VALUE object);

/** Converts a ruby object into a joystick */
SDL_Joystick *ray_rb2joystick(VALUE object);

/* Initializers */

void Init_ray_ext();
void Init_ray_image();
void Init_ray_color();
void Init_ray_rect();
void Init_ray_event();
void Init_ray_joystick();

#ifdef PSP
void Init_ray_psp();
#endif

#ifdef __cplusplus
#if 0
{
#endif
}
#endif

#endif
