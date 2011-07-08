/* -*- mode: objc -*- */

static void say_osx_setup_process() {
  static bool started = false;

  if (!started) {
    started = true;

    ProcessSerialNumber psn;
    if (GetCurrentProcess(&psn) == 0) {
      TransformProcessType(&psn, kProcessTransformToForegroundApplication);
      SetFrontProcess(&psn);
    }

    [[NSApplication sharedApplication] finishLaunching];
  }
}

@interface SayWindowImp : NSWindow
@end

@implementation SayWindowImp
- (BOOL)canBecomeKeyWindow  { return YES; }
- (BOOL)canBecomeMainWindow { return YES; }
@end

@interface SayWindow ()
- (void)ownMouseDown:(NSEvent*)nsev;
- (void)ownMouseUp:(NSEvent*)nsev;

- (void)ownRightMouseDown:(NSEvent*)nsev;
- (void)ownRightMouseUp:(NSEvent*)nsev;

- (void)ownOtherMouseDown:(NSEvent*)nsev;
- (void)ownOtherMouseUp:(NSEvent*)nsev;

- (void)ownScrollWheel:(NSEvent*)nsev;
@end

static uint8_t say_osx_convert_mod(NSEvent *ev);

@implementation SayWindow

@synthesize view;

- (id)init {
  if (!(self = [super init]))
    return nil;

  events = say_array_create(sizeof(say_event), NULL, NULL);
  return self;
}

- (BOOL)openWithTitle:(const char*)title
                width:(size_t)w
               height:(size_t)h
                style:(uint8_t)style {
  if ([NSThread currentThread] != [NSThread mainThread]) {
    say_error_set("can't create window outside main thread");
    return NO;
  }

  say_osx_setup_process();

  NSRect rect = NSMakeRect(0, 0, w, h);

  unsigned int cocoa_style = NSBorderlessWindowMask;

  if (!((style & SAY_WINDOW_NO_FRAME) || (style & SAY_WINDOW_FULLSCREEN))) {
    cocoa_style |= (NSTitledWindowMask |
                    NSMiniaturizableWindowMask |
                    NSClosableWindowMask);

    if (style & SAY_WINDOW_RESIZABLE) {
      cocoa_style |= NSResizableWindowMask;
    }
  }

  if (style & SAY_WINDOW_FULLSCREEN) {
    rect = [[NSScreen mainScreen] frame];

    real_w = w;
    real_h = h;

    screen_w = rect.size.width;
    screen_h = rect.size.height;
  }

  allow_close = NO;

  window = [[SayWindowImp alloc] initWithContentRect:rect
                                           styleMask:cocoa_style
                                             backing:NSBackingStoreBuffered
                                               defer:NO];

  if (!window) {
    say_error_set("could not create window");
    return NO;
  }

  /*
   * Beware: this must be called before setting the content view.
   * Otherwise, no drawing will happen.
   */
  [window makeKeyAndOrderFront:nil];

  if (style & SAY_WINDOW_FULLSCREEN) {
    [window setOpaque:YES];
    [window setHidesOnDeactivate:YES];
    [window setLevel:NSMainMenuWindowLevel+1];
    [NSMenu setMenuBarVisible:NO];
  }

  [window center];

  if (view) { [view release]; }

  view = [[NSOpenGLView alloc] initWithFrame:rect
                                 pixelFormat:[SayContext format]];

  track = [view addTrackingRect:view.frame
                          owner:self
                       userData:nil
                   assumeInside:YES];

  [window setContentView:view];

  [window setAcceptsMouseMovedEvents:YES];
  [window setIgnoresMouseEvents:NO];
  [window setAutodisplay:YES];
  [window setReleasedWhenClosed:NO];

  [window makeFirstResponder:self];

  [window setDelegate:self];

  [self setTitle:title];

  modifiers = say_osx_convert_mod([NSApp currentEvent]);

  return YES;
}

- (void)setIcon:(say_image*)img {
  id pool = [NSAutoreleasePool new];

  size_t w = say_image_get_width(img);
  size_t h = say_image_get_width(img);

  NSBitmapImageRep *rep;
  rep = [[NSBitmapImageRep alloc] initWithBitmapDataPlanes:0
                                                pixelsWide:w
                                                pixelsHigh:h
                                             bitsPerSample:8

                                           samplesPerPixel:4
                                                  hasAlpha:YES
                                                  isPlanar:NO

                                            colorSpaceName:NSCalibratedRGBColorSpace
                                               bytesPerRow:0

                                              bitsPerPixel:0];


  say_color *buf = say_image_get_buffer(img);
  for (size_t y = 0; y < h; y++) {
    for (size_t x = 0; x < w; x++) {
      say_color col = buf[x + y * w];
      NSUInteger pixel[] = {col.r, col.g, col.b, col.a};

      [rep setPixel:pixel atX:x y:y];
    }
  }

  NSImage *icon = [[NSImage alloc] initWithSize:NSMakeSize(w, h)];
  [icon addRepresentation:rep];

  [NSApp setApplicationIconImage:icon];

  [icon release];
  [rep release];

  [pool drain];
}

- (void)setTitle:(const char*)title {
  NSString *str = [[NSString alloc] initWithUTF8String:title];
  if (str)
    [window setTitle:str];

  [str release];
}

- (void)updateContext:(NSOpenGLContext*)context {
  if (real_w == 0 || real_h == 0)
    return;

  CGLContextObj cgcontext = (CGLContextObj)[context CGLContextObj];

  GLint size[] = {real_w, real_h};

  CGLSetParameter(cgcontext, kCGLCPSurfaceBackingSize, size);
  CGLEnable(cgcontext, kCGLCESurfaceBackingSize);
}

- (void)resizeToWidth:(size_t)w height:(size_t)h {
  if (real_w != 0 && real_h != 0) {
    real_w = w;
    real_h = h;

    [self updateContext:[view openGLContext]];

    say_event ev;
    ev.type = SAY_EVENT_RESIZE;
    ev.ev.resize.size = say_make_vector2(w, h);
    say_array_push(events, &ev);
  }
  else {
    NSRect frame = view.frame;
    frame.size = NSMakeSize(w, h);

    NSUInteger style = window.styleMask;
    NSRect window_frame = [NSWindow frameRectForContentRect:frame
                                                  styleMask:style];

    window_frame.origin = window.frame.origin;
    [window setFrame:window_frame display:NO animate:NO];
  }
}

- (void)close {
  if (window) {
     allow_close = YES;
    [window close];
    [window release];
    window = nil;
  }
}

- (void)dealloc {
  NSAutoreleasePool *pool = [NSAutoreleasePool new];

  [self close];
  [view release];
  say_array_free(events);
  [super dealloc];

  [pool drain];
}

/*
 * Event handling
 */

static NSEvent *say_osx_get_event(bool patient) {
  NSDate *date = patient ? [NSDate distantFuture] :
    [NSDate distantPast];

  return [NSApp nextEventMatchingMask:NSAnyEventMask
                            untilDate:date
                               inMode:NSDefaultRunLoopMode
                              dequeue:YES];
}

- (void)handleEvent:(NSEvent*)nsev {
  [NSApp sendEvent:nsev];

  if (nsev.window != window)
    return;

  /*
   * I can't find out how to register for those events.
   * They are thus dispatched manually.
   */

  switch (nsev.type) {
  case NSLeftMouseDown:  [self ownMouseDown:nsev];      break;
  case NSLeftMouseUp:    [self ownMouseUp:nsev];        break;

  case NSRightMouseDown: [self ownRightMouseDown:nsev]; break;
  case NSRightMouseUp:   [self ownRightMouseUp:nsev];   break;

  case NSOtherMouseDown: [self ownOtherMouseDown:nsev]; break;
  case NSOtherMouseUp:   [self ownOtherMouseUp:nsev];   break;

  case NSLeftMouseDragged:  /* F             */
  case NSRightMouseDragged: /* F             */
  case NSOtherMouseDragged: /* FALL THROUGH! */
    [self mouseMoved:nsev];
    break;

  case NSScrollWheel:
    [self ownScrollWheel:nsev];
    break;

  default: break;
  }
}

- (BOOL)pollEvent:(say_event*)ev {
  NSEvent *nsev = nil;

  NSAutoreleasePool *pool = [NSAutoreleasePool new];
  if ((nsev = say_osx_get_event(false))) {
    [self handleEvent:nsev];
  }
  [pool drain];

  if (say_array_get_size(events) != 0) {
    *ev = *(say_event*)say_array_get(events, 0);
    say_array_delete(events, 0);

    return YES;
  }
  else
    ev->type = SAY_EVENT_NONE;

  return NO;
}

- (void)waitEvent:(say_event*)ev {
  NSEvent *nsev = nil;

  NSAutoreleasePool *pool = [NSAutoreleasePool new];

  while (say_array_get_size(events) == 0 && (nsev = say_osx_get_event(true))) {
    [NSApp sendEvent:nsev];
  }

  [pool drain];

  *ev = *(say_event*)say_array_get(events, 0);
  say_array_delete(events, 0);
}

static say_key say_osx_convert_key(NSEvent *ev) {
  NSString *str = [ev charactersIgnoringModifiers];

  if ([str length] == 0)
    return 0;

  unichar chr = [str characterAtIndex:0];

  /* Downcase ASCII */
  if (chr >= 'A' && chr <= 'Z')
    chr += 'a' - 'A';

  switch (chr) {
  case 'a': return SAY_KEY_A;
  case 'b': return SAY_KEY_B;
  case 'c': return SAY_KEY_C;
  case 'd': return SAY_KEY_D;
  case 'e': return SAY_KEY_E;
  case 'f': return SAY_KEY_F;
  case 'g': return SAY_KEY_G;
  case 'h': return SAY_KEY_H;
  case 'i': return SAY_KEY_I;
  case 'j': return SAY_KEY_J;
  case 'k': return SAY_KEY_K;
  case 'l': return SAY_KEY_L;
  case 'm': return SAY_KEY_M;
  case 'n': return SAY_KEY_N;
  case 'o': return SAY_KEY_O;
  case 'p': return SAY_KEY_P;
  case 'q': return SAY_KEY_Q;
  case 'r': return SAY_KEY_R;
  case 's': return SAY_KEY_S;
  case 't': return SAY_KEY_T;
  case 'u': return SAY_KEY_U;
  case 'v': return SAY_KEY_V;
  case 'w': return SAY_KEY_W;
  case 'x': return SAY_KEY_X;
  case 'y': return SAY_KEY_Y;
  case 'z': return SAY_KEY_Z;

  case '0': return SAY_KEY_NUM_0;
  case '1': return SAY_KEY_NUM_1;
  case '2': return SAY_KEY_NUM_2;
  case '3': return SAY_KEY_NUM_3;
  case '4': return SAY_KEY_NUM_4;
  case '5': return SAY_KEY_NUM_5;
  case '6': return SAY_KEY_NUM_6;
  case '7': return SAY_KEY_NUM_7;
  case '8': return SAY_KEY_NUM_8;
  case '9': return SAY_KEY_NUM_9;

  case NSF1FunctionKey:  return SAY_KEY_F1;
  case NSF2FunctionKey:  return SAY_KEY_F2;
  case NSF3FunctionKey:  return SAY_KEY_F3;
  case NSF4FunctionKey:  return SAY_KEY_F4;
  case NSF5FunctionKey:  return SAY_KEY_F5;
  case NSF6FunctionKey:  return SAY_KEY_F6;
  case NSF7FunctionKey:  return SAY_KEY_F7;
  case NSF8FunctionKey:  return SAY_KEY_F8;
  case NSF9FunctionKey:  return SAY_KEY_F9;
  case NSF10FunctionKey: return SAY_KEY_F10;
  case NSF11FunctionKey: return SAY_KEY_F11;
  case NSF12FunctionKey: return SAY_KEY_F12;
  case NSF13FunctionKey: return SAY_KEY_F13;
  case NSF14FunctionKey: return SAY_KEY_F14;
  case NSF15FunctionKey: return SAY_KEY_F15;

  case NSMenuFunctionKey: return SAY_KEY_MENU;
  case '[':  return SAY_KEY_LBRACKET;
  case ']':  return SAY_KEY_RBRACKET;
  case ';':  return SAY_KEY_SEMICOLON;
  case ',':  return SAY_KEY_COMMA;
  case '.':  return SAY_KEY_PERIOD;
  case '"':  return SAY_KEY_QUOTE;
  case '/':  return SAY_KEY_SLASH;
  case '\\': return SAY_KEY_BACKSLASH;
  case '~':  return SAY_KEY_TILDE;
  case '=':  return SAY_KEY_EQUAL;
  case '-':  return SAY_KEY_MINUS;
  case '+':  return SAY_KEY_PLUS;
  case '*':  return SAY_KEY_ASTERISK;
  case ' ':  return SAY_KEY_SPACE;

  case NSLeftArrowFunctionKey:  return SAY_KEY_LEFT;
  case NSRightArrowFunctionKey: return SAY_KEY_RIGHT;
  case NSUpArrowFunctionKey:    return SAY_KEY_UP;
  case NSDownArrowFunctionKey:  return SAY_KEY_DOWN;

  case '\e': return SAY_KEY_ESCAPE;
  case  127: return SAY_KEY_BACKSPACE;
  case '\t': return SAY_KEY_TAB;
  case '\r': return SAY_KEY_RETURN;
  case '\n': return SAY_KEY_RETURN;
  case NSPageUpFunctionKey: return SAY_KEY_PAGE_UP;
  case NSPageDownFunctionKey: return SAY_KEY_PAGE_DOWN;
  case NSEndFunctionKey: return SAY_KEY_END;
  case NSHomeFunctionKey: return SAY_KEY_HOME;
  case NSInsertFunctionKey: return SAY_KEY_INSERT;
  case NSDeleteFunctionKey: return SAY_KEY_DELETE;
  case NSPauseFunctionKey: return SAY_KEY_PAUSE;

  default: return SAY_KEY_UNKNOWN;
  }
}

static uint8_t say_osx_convert_mod(NSEvent *ev) {
  uint8_t    ret = 0;
  NSUInteger mod = ev.modifierFlags;

  if (mod & NSShiftKeyMask)     ret |= SAY_MOD_SHIFT;
  if (mod & NSAlternateKeyMask) ret |= SAY_MOD_META;
  if (mod & NSControlKeyMask)   ret |= SAY_MOD_CONTROL;
  if (mod & NSCommandKeyMask)   ret |= SAY_MOD_SUPER;

  return ret;
}

/*
 * NSResponder
 */

- (BOOL)acceptsFirstResponder { return YES; }
- (BOOL)becomeFirstResponder  { return YES; }
- (BOOL)resignFirstResponder  { return  NO; }

- (void)keyDown:(NSEvent*)nsev {
  if ([nsev isARepeat])
    return;

  say_event ev;
  ev.type = SAY_EVENT_KEY_PRESS;

  ev.ev.key.code        = say_osx_convert_key(nsev);
  ev.ev.key.mod         = say_osx_convert_mod(nsev);
  ev.ev.key.native_code = nsev.keyCode;

  say_array_push(events, &ev);

  NSText *text = [window fieldEditor:YES forObject:self];
  [text interpretKeyEvents:[NSArray arrayWithObject:nsev]];

  if (text.string.length != 0) {
    ev.type = SAY_EVENT_TEXT_ENTERED;
    ev.ev.text.text = [text.string characterAtIndex:0];

    say_array_push(events, &ev);

    text.string = @"";
  }
}

- (void)keyUp:(NSEvent*)nsev {
  say_event ev;
  ev.type = SAY_EVENT_KEY_RELEASE;

  ev.ev.key.code        = say_osx_convert_key(nsev);
  ev.ev.key.mod         = say_osx_convert_mod(nsev);
  ev.ev.key.native_code = nsev.keyCode;

  say_array_push(events, &ev);
}

- (void)submitFlagChange:(uint8_t)mod forKey:(say_key)key
                   press:(BOOL)press {
  say_event ev;
  ev.type = press ? SAY_EVENT_KEY_PRESS : SAY_EVENT_KEY_RELEASE;

  ev.ev.key.code        = key;
  ev.ev.key.mod         = mod;
  ev.ev.key.native_code = 0;

  say_array_push(events, &ev);
}

- (void)flagsChanged:(NSEvent*)nsev {
  uint8_t new_mod = say_osx_convert_mod(nsev);
  if (new_mod == modifiers)
    return;

  bool meta_was_enabled = (modifiers & SAY_MOD_META) ? true : false;
  bool meta_is_enabled  = (new_mod   & SAY_MOD_META) ? true : false;

  if (meta_was_enabled != meta_is_enabled) {
    [self submitFlagChange:new_mod forKey:SAY_KEY_LMETA
                     press:meta_is_enabled];
  }

  bool shift_was_enabled = (modifiers & SAY_MOD_SHIFT) ? true : false;
  bool shift_is_enabled  = (new_mod   & SAY_MOD_SHIFT) ? true : false;

  if (shift_was_enabled != shift_is_enabled) {
    [self submitFlagChange:new_mod forKey:SAY_KEY_LSHIFT
                     press:shift_is_enabled];
  }

  bool ctrl_was_enabled = (modifiers & SAY_MOD_CONTROL) ? true : false;
  bool ctrl_is_enabled  = (new_mod   & SAY_MOD_CONTROL) ? true : false;

  if (ctrl_was_enabled != ctrl_is_enabled) {
    [self submitFlagChange:new_mod forKey:SAY_KEY_LCONTROL
                     press:ctrl_is_enabled];
  }

  bool super_was_enabled = (modifiers & SAY_MOD_SUPER) ? true : false;
  bool super_is_enabled  = (new_mod   & SAY_MOD_SUPER) ? true : false;

  if (super_was_enabled != super_is_enabled) {
    [self submitFlagChange:new_mod forKey:SAY_KEY_LSUPER
                     press:super_is_enabled];
  }

  modifiers = new_mod;
}

- (say_vector2)convertPoint:(NSPoint)point {
  point   = [view convertPoint:point fromView:nil];
  point.y = view.frame.size.height - point.y;

  /* Convert to real position in fullscreen mode */
  if (real_w != 0 && real_h != 0) {
    point.x = (point.x / screen_w) * real_w;
    point.y = (point.y / screen_h) * real_h;
  }

  return say_make_vector2(point.x, point.y);
}

- (void)ownMouseDown:(NSEvent*)nsev {
  say_event ev;
  ev.type = SAY_EVENT_BUTTON_PRESS;
  ev.ev.button.pos = [self convertPoint:nsev.locationInWindow];
  ev.ev.button.button = SAY_BUTTON_LEFT;
  say_array_push(events, &ev);
}

- (void)ownMouseUp:(NSEvent*)nsev {
  say_event ev;
  ev.type = SAY_EVENT_BUTTON_RELEASE;
  ev.ev.button.pos = [self convertPoint:nsev.locationInWindow];
  ev.ev.button.button = SAY_BUTTON_LEFT;
  say_array_push(events, &ev);
}

- (void)ownRightMouseDown:(NSEvent*)nsev {
  say_event ev;
  ev.type = SAY_EVENT_BUTTON_PRESS;
  ev.ev.button.pos = [self convertPoint:nsev.locationInWindow];
  ev.ev.button.button = SAY_BUTTON_RIGHT;
  say_array_push(events, &ev);
}

- (void)ownRightMouseUp:(NSEvent*)nsev {
  say_event ev;
  ev.type = SAY_EVENT_BUTTON_RELEASE;
  ev.ev.button.pos = [self convertPoint:nsev.locationInWindow];
  ev.ev.button.button = SAY_BUTTON_RIGHT;
  say_array_push(events, &ev);
}

/* Assume other mouse button will be middle. */
- (void)ownOtherMouseDown:(NSEvent*)nsev {
  say_event ev;
  ev.type = SAY_EVENT_BUTTON_PRESS;
  ev.ev.button.pos = [self convertPoint:nsev.locationInWindow];
  ev.ev.button.button = SAY_BUTTON_MIDDLE;
  say_array_push(events, &ev);
}

- (void)ownOtherMouseUp:(NSEvent*)nsev {
  say_event ev;
  ev.type = SAY_EVENT_BUTTON_RELEASE;
  ev.ev.button.pos = [self convertPoint:nsev.locationInWindow];
  ev.ev.button.button = SAY_BUTTON_MIDDLE;
  say_array_push(events, &ev);
}

- (void)mouseMoved:(NSEvent*)nsev {
  say_event ev;
  ev.type = SAY_EVENT_MOUSE_MOTION;
  ev.ev.motion.pos = [self convertPoint:nsev.locationInWindow];
  say_array_push(events, &ev);
}

- (void)mouseEntered:(NSEvent*)nsev {
  say_event ev;
  ev.type = SAY_EVENT_MOUSE_ENTERED;
  say_array_push(events, &ev);
}

- (void)mouseExited:(NSEvent*)nsev {
  say_event ev;
  ev.type = SAY_EVENT_MOUSE_LEFT;
  say_array_push(events, &ev);
}

- (void)ownScrollWheel:(NSEvent*)nsev {
  if (nsev.deltaY == 0)
    return;

  say_event ev;
  ev.type = SAY_EVENT_WHEEL_MOTION;
  ev.ev.wheel.pos = [self convertPoint:nsev.locationInWindow];
  ev.ev.wheel.delta = nsev.deltaY;
  say_array_push(events, &ev);
}

/*
 *  Window delegate
 */

- (BOOL)windowShouldClose:(id)sender {
  say_event ev;
  ev.type = SAY_EVENT_QUIT;
  say_array_push(events, &ev);

  if (allow_close) {
    allow_close = NO;
    return YES;
  }
  else
    return NO;
}

- (void)windowDidBecomeKey:(NSNotification*)not {
  say_event ev;
  ev.type = SAY_EVENT_FOCUS_GAIN;
  say_array_push(events, &ev);
}

- (void)windowDidResignKey:(NSNotification*)not {
  say_event ev;
  ev.type = SAY_EVENT_FOCUS_LOSS;
  say_array_push(events, &ev);
}

- (void)windowDidResize:(NSNotification*)not {
  NSSize size = view.frame.size;

  /* Ignore resize events in fullscreen windows */
  if (real_w == 0 && real_h == 0) {
    say_event ev;
    ev.type = SAY_EVENT_RESIZE;
    ev.ev.resize.size = say_make_vector2(size.width, size.height);
    say_array_push(events, &ev);
  }

  [view removeTrackingRect:track];
  track = [view addTrackingRect:view.frame
                          owner:self
                       userData:nil
                   assumeInside:YES];

  [view update];
}

@end

say_imp_window say_imp_window_create() {
  return [SayWindow new];
}

void say_imp_window_free(say_imp_window win) {
  [win release];
}

bool say_imp_window_open(say_imp_window win,
                         const char *title,
                         size_t w, size_t h,
                         uint8_t flags) {
  return [win openWithTitle:title
                      width:w
                     height:h
                      style:flags];
}

void say_imp_window_close(say_imp_window win) {
  [win close];
}

void say_imp_window_show_cursor(say_imp_window win) {
  [NSCursor unhide];
}

void say_imp_window_hide_cursor(say_imp_window win) {
  [NSCursor hide];
}

void say_imp_window_set_icon(say_imp_window win, struct say_image *img) {
  [win setIcon:img];
}

void say_imp_window_set_title(say_imp_window win, const char *title) {
  [win setTitle:title];
}

void say_imp_window_resize(say_imp_window win, size_t w, size_t h) {
  [win resizeToWidth:w height:h];
}

bool say_imp_window_poll_event(say_imp_window win, struct say_event *ev) {
  return [win pollEvent:ev];
}

void say_imp_window_wait_event(say_imp_window win, struct say_event *ev) {
  [win waitEvent:ev];
}
