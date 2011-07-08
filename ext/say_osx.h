/* -*- mode: objc -*- */

#import <Cocoa/Cocoa.h>

struct say_event;
struct say_array;
struct say_image;

@class SayWindow;

@interface SayContext : NSObject {
  NSOpenGLContext *context;
  SayWindow *win;
}

+ (NSOpenGLPixelFormat*)format;

- (id)initWithShared:(SayContext*)shared;
- (void)setWindow:(SayWindow*)win;

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

  GLint real_w, real_h, screen_w, screen_h;
}

- (BOOL)openWithTitle:(const char*)title
                width:(size_t)w
               height:(size_t)h
                style:(uint8_t)style;
- (void)close;

- (void)updateContext:(NSOpenGLContext*)context;

- (void)setIcon:(struct say_image*)img;
- (void)setTitle:(const char*)title;
- (void)resizeToWidth:(size_t)w height:(size_t)h;

- (BOOL)pollEvent:(struct say_event*)ev;
- (void)waitEvent:(struct say_event*)ev;

@property (readonly) NSOpenGLView *view;

@end
