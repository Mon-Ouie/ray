#include "ray.h"

#ifdef HAVE_COCOA
extern void ray_osx_init();
extern void ray_osx_close();
#endif

VALUE ray_mRay = Qnil;

/* Inits ray */
VALUE ray_init(VALUE self) {
#ifdef HAVE_COCOA
   ray_osx_init();
#endif

   SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
   return Qnil;
}

/* Stops ray */
VALUE ray_stop(VALUE self) {
   SDL_Quit();
   /* The pool is never drained on OSX */
   return Qnil;
}

ray_video_mode ray_parse_video_mode(VALUE hash) {
   VALUE width  = rb_hash_aref(hash, RAY_SYM("width"));
   VALUE height = rb_hash_aref(hash, RAY_SYM("height"));

   if (NIL_P(width))  width  = rb_hash_aref(hash, RAY_SYM("w"));
   if (NIL_P(height)) height = rb_hash_aref(hash, RAY_SYM("h"));

   if (NIL_P(width) || NIL_P(height))
      rb_raise(rb_eArgError, "Missing parameter: width or height");

   VALUE bitsperpixel = rb_hash_aref(hash, RAY_SYM("bits_per_pixel"));
   if (NIL_P(bitsperpixel)) bitsperpixel = rb_hash_aref(hash, RAY_SYM("bpp"));

   if (NIL_P(bitsperpixel))
      bitsperpixel = INT2FIX(32);

   uint32_t flags = 0;
   if (RTEST(rb_hash_aref(hash, RAY_SYM("sw_surface"))))
      flags |= SDL_SWSURFACE;
   else
      flags |= SDL_HWSURFACE; /* even if hwsurface was false and not nil */

   if (RTEST(rb_hash_aref(hash, RAY_SYM("async_blit"))))
      flags |= SDL_ASYNCBLIT;

   if (!(flags & SDL_SWSURFACE) &&
       RTEST(rb_hash_aref(hash, RAY_SYM("double_buf")))) {
      flags |= SDL_DOUBLEBUF;
   }

   if (RTEST(rb_hash_aref(hash, RAY_SYM("fullscreen"))))
      flags |= SDL_FULLSCREEN;

   if (RTEST(rb_hash_aref(hash, RAY_SYM("no_frame"))))
      flags |= SDL_NOFRAME;

   if (RTEST(rb_hash_aref(hash, RAY_SYM("resizable"))))
      flags |= SDL_RESIZABLE;

   ray_video_mode mode = {
      NUM2INT(width),
      NUM2INT(height),

      NUM2INT(bitsperpixel),

      flags
   };

   return mode;
}

/*
  Creates a new window.

  @note If both hw_surface and sws_urface are false, hw_surface
        will be considered as true. :sw_surface should be true
        if you want to acess

  @return [Ray::Image] An image representing the window

  @option hash [Integer] :width Width of the window
  @option hash [Integer] :height Height of the window

  @option hash [Integer] :w Alias for width
  @option hash [Integer] :h Alias for height

  @option hash [Integer] :bits_per_pixel Bits per pixel. Valid values are
                                         8, 15, 16, 24, and 32.
  @option hash [Integer] :bpp Alias for bits_per_pixel

  @option hash [true, false] :hw_surface Creates the surface in video memory
                                        (default)
  @option hash [true, false] :sw_surface Creates the surface in system memory
  @option hash [true, false] :async_blit Enables asynchronous updates of the
                                         the surface
  @option hash [true, false] :double_buf Enables double buffering. Ignored
                                         if sw_surface is set.
  @option hash [true, false] :fullscreen Creates a full screen window.
  @option hash [true, false] :resizable  Creates a resizable window.
  @option hash [true, false] :no_frame   Disables window decoration if
                                         possible.
 */
VALUE ray_create_window(VALUE self, VALUE hash) {
   ray_video_mode mode = ray_parse_video_mode(hash);
   SDL_Surface *screen = SDL_SetVideoMode(mode.width, mode.height,
                                          mode.bpp, mode.flags);
   
   if (!screen) {
      rb_raise(rb_eRuntimeError, "Could not create the window (%s)",
               SDL_GetError());
   }

   return ray_create_image(screen);
}

/*
  @return [true, false] True if the video mode described by hash
                        can be used.
*/
VALUE ray_can_use_mode(VALUE self, VALUE hash) {
   ray_video_mode mode = ray_parse_video_mode(hash);
   
   int res = SDL_VideoModeOK(mode.width, mode.height, mode.bpp, mode.flags);
   return res ? Qtrue : Qfalse;
}

/*
  Sets the window icon
  @param [Ray::Image] icon The icon to display
*/
VALUE ray_set_icon(VALUE self, VALUE icon) {
   SDL_WM_SetIcon(ray_rb2surface(icon), NULL);
   return icon;
}

/* @return [String, nil] The window title */
VALUE ray_window_title(VALUE self) {
   char *title = NULL;
   SDL_WM_GetCaption(&title, NULL);

   if (!title)
      return Qnil;
   return rb_str_new2(title);
}

/* Sets the window title */
VALUE ray_set_window_title(VALUE self, VALUE title) {
   char *icon = NULL;

   if (!NIL_P(title)) title = rb_String(title);

   SDL_WM_GetCaption(NULL, &icon);
   SDL_WM_SetCaption(NIL_P(title) ? NULL : StringValuePtr(title), icon);

   return title;
}

/* @return [String, nil] The window text icon */
VALUE ray_text_icon(VALUE self) {
   char *icon = NULL;
   SDL_WM_GetCaption(NULL, &icon);

   if (!icon)
      return Qnil;
   return rb_str_new2(icon);   
}

/* Sets the window title */
VALUE ray_set_text_icon(VALUE self, VALUE icon) {
   char *title;
   
   if (!NIL_P(icon)) icon = rb_String(icon);

   SDL_WM_GetCaption(&title, NULL);
   SDL_WM_SetCaption(title, NIL_P(icon) ? NULL : StringValuePtr(icon));

   return icon;
}

/*
  @return [true, false] True if the input is grabbed, which means the mouse
                        is confined in the window, and keyboard input is sent
                        directly to the window.
*/
VALUE ray_grab_input(VALUE self) {
   SDL_GrabMode mode = SDL_WM_GrabInput(SDL_GRAB_QUERY);
   return (mode == SDL_GRAB_ON) ? Qtrue : Qfalse;
}

/* Sets the grab input to true or false */
VALUE ray_set_grab_input(VALUE self, VALUE grab) {
   SDL_WM_GrabInput(RTEST(grab) ? SDL_GRAB_ON : SDL_GRAB_OFF);
   return grab;
}

/* @return [Ray::Image, nil] The current screen, created by create_window */
VALUE ray_screen(VALUE self) {
   SDL_Surface *surf = SDL_GetVideoSurface();
   
   if (!surf)
      return Qnil;
   return ray_create_image(surf);
}

/* @return [true, false] true if Ray supports other image formats than BMP */
VALUE ray_has_image_support(VALUE self) {
#ifdef HAVE_SDL_IMAGE
   return Qtrue;
#else
   return Qfalse;
#endif
}
/* @return [true, false] true if Ray supports graphical effect like rotations */
VALUE ray_has_gfx_support(VALUE self) {
#ifdef HAVE_SDL_GFX
   return Qtrue;
#else
   return Qfalse;
#endif
}

void Init_ray_ext() {
   ray_mRay = rb_define_module("Ray");

   rb_define_module_function(ray_mRay, "init", ray_init, 0);
   rb_define_module_function(ray_mRay, "stop", ray_stop, 0);

   rb_define_module_function(ray_mRay, "create_window", ray_create_window, 1);
   rb_define_module_function(ray_mRay, "can_use_mode?", ray_can_use_mode, 1);
   rb_define_module_function(ray_mRay, "screen", ray_screen, 0);
   
   rb_define_module_function(ray_mRay, "icon=", ray_set_icon, 1);
   rb_define_module_function(ray_mRay, "window_title=", ray_set_window_title,
                             1);
   rb_define_module_function(ray_mRay, "text_icon=", ray_set_text_icon, 1);

   rb_define_module_function(ray_mRay, "window_title", ray_window_title, 0);
   rb_define_module_function(ray_mRay, "text_icon", ray_text_icon, 0);

   rb_define_module_function(ray_mRay, "grab_input", ray_grab_input, 0);
   rb_define_module_function(ray_mRay, "grab_input=", ray_set_grab_input, 1);

   rb_define_module_function(ray_mRay, "has_image_support?",
                             ray_has_image_support, 0);
   rb_define_module_function(ray_mRay, "has_gfx_support?",
                             ray_has_gfx_support, 0);

   Init_ray_image();
   Init_ray_color();
   Init_ray_rect();
   Init_ray_event();
   Init_ray_joystick();

#ifdef PSP
   Init_ray_psp();
#endif
}

#ifdef PSP

PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
PSP_HEAP_SIZE_KB(-512);

#include <unistd.h>

#define RGB(r, g, b) ((r) | ((g)<<8) | ((b)<<16) | (0xff<<24))

VALUE ray_safe_load(VALUE self) {
   return rb_require("./script.rb");
}

int SDL_main(int argc, char *argv[]) {
   ruby_init();

   ruby_incpush("./ruby/1.8");
   ruby_incpush("./ruby/site_ruby");
   ruby_incpush("./ruby/site_ruby/1.8");

   ruby_incpush("ms0:/ruby/1.8");
   ruby_incpush("ms0:/ruby/site_ruby");
   ruby_incpush("ms0:/ruby/site_ruby/1.8");

   ruby_incpush(".");
   
   ruby_script("ray");

   Init_ray_ext();

   int error = 0;
   VALUE res = rb_protect(ray_safe_load, 0, &error);

   if (error != 0) {
      pspDebugScreenInit();

      pspDebugScreenSetBackColor(RGB(0, 0, 0));
      pspDebugScreenEnableBackColor(1);

      pspDebugScreenClear();
      pspDebugScreenSetXY(0, 0);

      pspDebugScreenSetTextColor(RGB(255, 0, 0));
      pspDebugScreenPrintf("Ray - error manager\n\n\n");
      pspDebugScreenSetTextColor(RGB(255, 255, 255));

      pspDebugScreenPrintf("This is Ray's error manager.An exception ");
      pspDebugScreenPrintf("has been thrown, and the script ");
      pspDebugScreenPrintf("cannot be run. If you're the developper of");
      pspDebugScreenPrintf(" this application,\ncheck yout script.");
      pspDebugScreenPrintf(" If it seems fine to you, consider ");
      pspDebugScreenPrintf("filling a bug report.\n");
      pspDebugScreenPrintf("If you're not the developper, consider");
      pspDebugScreenPrintf(" contacting him.\n\n\n");

      pspDebugScreenSetTextColor(RGB(255, 0, 0));
      pspDebugScreenPrintf("Errors informations :\n\n\n");
      pspDebugScreenSetTextColor(RGB(255, 255, 255));

      // Now, we get the error.
      VALUE error = rb_gv_get("$!");

      // And the information we want to have about it.
      VALUE backtrace = rb_funcall(error, RAY_METH("backtrace"), 0);
      VALUE type      = rb_funcall(error, RAY_METH("class"), 0);
      VALUE msg       = rb_funcall(error, RAY_METH("message"), 0);

      type = rb_funcall(type, RAY_METH("to_s"), 0);

      VALUE first = rb_ary_entry(backtrace, 0);

      FILE *log = fopen("ray-error.log", "w");
      if (!log) {
         sceKernelExitGame();
         return 1;
      }

      pspDebugScreenPrintf("%s: %s: %s\n", StringValuePtr(first),
                                           StringValuePtr(type),
                                           StringValuePtr(msg));
      fprintf(log, "%s: %s: %s\n", StringValuePtr(first), StringValuePtr(type),
              StringValuePtr(msg));

      int i = 1;
      for (i = 1; i < RARRAY_LEN(backtrace); ++i) {
         VALUE entry = rb_ary_entry(backtrace, i);
         pspDebugScreenPrintf("%s\n", StringValuePtr(entry));
         fprintf(log, "%s\n", StringValuePtr(entry));
      }

      fclose(log);

      pspDebugScreenPrintf("\nThese informations can be found in ");
      pspDebugScreenPrintf("ray-error.log\n\n");

      pspDebugScreenPrintf("Press X to quit.");

      SceCtrlData pad;

      sceCtrlSetSamplingCycle(0);
      sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
      sceCtrlReadBufferPositive(&pad, 1);

      while (!(pad.Buttons & PSP_CTRL_CROSS))
         sceCtrlReadBufferPositive(&pad, 1);

      sceKernelExitGame();
      return 0;
   }

   return res;
}

#endif
