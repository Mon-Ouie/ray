/* -*- mode: objc -*- */

#import <Cocoa/Cocoa.h>

struct say_event;
struct say_array;

@interface SayContext : NSObject {
  NSOpenGLContext *context;
}

+ (NSOpenGLPixelFormat*)format;

- (id)initWithShared:(SayContext*)shared;
- (void)setView:(NSOpenGLView*)view;

- (void)update;
- (void)makeCurrent;

@property (readonly) NSOpenGLContext *context;

@end

@interface SayWindow : NSResponder<NSWindowDelegate> {
  NSWindow *window;
  NSOpenGLView *view;

  struct say_array *events;

  BOOL allow_close;

  NSTrackingRectTag track;

  uint8_t modifiers;
}

- (BOOL)openWithTitle:(const char*)title
                width:(size_t)w
               height:(size_t)h
                style:(uint8_t)style;
- (void)close;

- (BOOL)pollEvent:(struct say_event*)ev;
- (void)waitEvent:(struct say_event*)ev;

@property (readonly) NSOpenGLView *view;

@end
