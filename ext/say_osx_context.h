/* -*- mode: objc -*- */

@implementation SayContext

@synthesize context;

+ (NSOpenGLPixelFormat*)format {
  static NSOpenGLPixelFormat *format = nil;

  if (!format) {
    NSOpenGLPixelFormatAttribute attr[] = {
      NSOpenGLPFAClosestPolicy,
      NSOpenGLPFADoubleBuffer,
      NSOpenGLPFAAccelerated,

      NSOpenGLPFAColorSize, 24,
      NSOpenGLPFAAlphaSize, 8,

      NSOpenGLPFADepthSize, 24,

      0
    };

    format = [[NSOpenGLPixelFormat alloc] initWithAttributes:attr];
  }

  return format;
}

- (id)initWithShared:(SayContext*)shared {
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
  NSAutoreleasePool *pool = [NSAutoreleasePool new];
  [context release];
  [super dealloc];
  [pool drain];
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
