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

- (void)setView:(NSOpenGLView*)view {
  [context setView:view];
}

- (void)update {
  [context flushBuffer];
}

- (void)makeCurrent {
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
