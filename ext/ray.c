#include "ray.h"

VALUE ray_mRay = Qnil;

/* Inits ray */
VALUE ray_init(VALUE self) {
   SDL_Init(SDL_INIT_VIDEO);
   return Qnil;
}

/* Stops ray */
VALUE ray_stop(VALUE self) {
   SDL_Quit();
   return Qnil;
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

   SDL_Surface *screen = SDL_SetVideoMode(NUM2INT(width),
                                          NUM2INT(height),
                                          NUM2INT(bitsperpixel),
                                          flags);
   if (!screen) {
      rb_raise(rb_eRuntimeError, "Could not create the window (%s)",
               SDL_GetError());
   }

   return ray_create_image(screen);
}

void Init_ray_ext() {
   ray_mRay = rb_define_module("Ray");

   rb_define_module_function(ray_mRay, "init", ray_init, 0);
   rb_define_module_function(ray_mRay, "stop", ray_stop, 0);
   rb_define_module_function(ray_mRay, "create_window", ray_create_window, 1);

   Init_ray_image();
   Init_ray_color();
   Init_ray_rect();
}

#ifndef PSP

int main(int argc, char *argv[]) {
#if defined(HAVE_RUBY_RUN_NODE)
   ruby_init();
   Init_ray_ext();
   ruby_run_node(ruby_options(argc, argv));
#elif defined(HAVE_RUBY_RUN)
   ruby_init();
   Init_ray_ext();
   ruby_init_loadpath();
   ruby_options(argc, argv);
   ruby_run();
#else
   fprintf(stderr, "Please use \"require 'ray'\" on this platform\n");
   return 1;
#endif

   return 0;
}

#endif

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

      for (int i = 1; i < RARRAY_LEN(backtrace); ++i) {
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
