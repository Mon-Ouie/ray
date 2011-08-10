/* -*- mode: objc -*- */

@implementation SayContext

@synthesize context;

+ (NSOpenGLPixelFormat*)format {
  static NSOpenGLPixelFormat *format = nil;

  if (!format) {
    say_context_config *conf = say_context_get_config();

    NSOpenGLPixelFormatAttribute attr[] = {
      NSOpenGLPFAClosestPolicy,
      NSOpenGLPFADoubleBuffer,
      NSOpenGLPFAAccelerated,

      NSOpenGLPFAColorSize, 24,
      NSOpenGLPFAAlphaSize, 8,

      /*
       * Next elements must be set depending on user configuration.
       */

      0, 0, /* depth */
      0, 0, /* stencil */

      0, 0, /* profile */

      0 /* terminator */
    };

    size_t i = 7;

    if (conf->depth_size) {
      attr[i + 0] = NSOpenGLPFADepthSize;
      attr[i + 1] = conf->depth_size;

      i += 2;
    }

    if (conf->stencil_size) {
      attr[i + 0] = NSOpenGLPFAStencilSize;
      attr[i + 1] = conf->stencil_size;

      i += 2;
    }

#ifdef AVAILABLE_MAC_OS_X_VERSION_10_7_AND_LATER
    if (conf->core_profile) { /* OpenGL 3.2! */
      attr[i + 0] = NSOpenGLPFAOpenGLProfile;
      attr[i + 1] = NSOpenGLProfileVersion3_2Core;

      i += 2;
    }
#endif

    format = [[NSOpenGLPixelFormat alloc] initWithAttributes:attr];
  }

  return format;
}

- (id)initWithShared:(SayContext*)shared {
  say_osx_flip_pool();

  if (!(self = [super init]))
    return nil;

  context = [[NSOpenGLContext alloc] initWithFormat:[SayContext format]
                                       shareContext:shared.context];

  if (!context) {
    say_error_set("could not create context");

    [self release];
    return nil;
  }

  return self;
}

- (void)setWindow:(SayWindow*)arg {
  win = arg;
  [context setView:win.view];
}

- (void)update {
  [context flushBuffer];
}

- (void)makeCurrent {
  [win updateContext:context];
  [(NSOpenGLView*)[context view] setOpenGLContext:context];
  [context makeCurrentContext];
}

- (void)dealloc {
  say_osx_flip_pool();
  [context release];
  [super dealloc];
}

@end

void say_imp_context_free(say_imp_context ctxt) {
  [ctxt release];
}

say_imp_context say_imp_context_create() {
  return [[SayContext alloc] initWithShared:nil];
}

say_imp_context say_imp_context_create_shared(say_imp_context shared) {
  return [[SayContext alloc] initWithShared:shared];
}

say_imp_context say_imp_context_create_for_window(say_imp_context shared,
                                                  say_imp_window  win) {
  SayContext *ctxt = [[SayContext alloc] initWithShared:shared];
  [ctxt setWindow:win];
  return ctxt;
}

void say_imp_context_make_current(say_imp_context ctxt) {
  [ctxt makeCurrent];
}

void say_imp_context_update(say_imp_context ctxt) {
  [ctxt update];
}
