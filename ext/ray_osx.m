#ifdef HAVE_SDL_H
# include <SDL.h>
#else
# include <SDL/SDL.h>
#endif

#include <Cocoa/Cocoa.h>

static NSAutoreleasePool *ray_pool = nil;

@interface NSApplication (SDL_Missing_Methods)
- (void)setAppleMenu:(NSMenu *)menu;
@end

@interface RayApplication : NSApplication
@end

@implementation RayApplication
- (void)terminate:(id)sender {
   SDL_Event event;
   event.type = SDL_QUIT;
   SDL_PushEvent(&event);
}
@end

NSString *ray_osx_appname() {
   const NSDictionary *dict;
   NSString *appName = 0;

   dict = (const NSDictionary *)CFBundleGetInfoDictionary(CFBundleGetMainBundle());
   if (dict)
      appName = [dict objectForKey: @"CFBundleName"];

   if ([appName length] == 0)
      appName = [[NSProcessInfo processInfo] processName];

   return appName;
}

void ray_osx_setup_application_menu() {
   NSMenu *appleMenu;
   NSMenuItem *menuItem;
   NSString *title;
   NSString *appName;

   appName = ray_osx_appname();
   appleMenu = [[NSMenu alloc] initWithTitle:@""];

   /* Add menu items */
   title = [@"About " stringByAppendingString:appName];
   [appleMenu addItemWithTitle:title action:@selector(orderFrontStandardAboutPanel:)
                 keyEquivalent:@""];

   [appleMenu addItem:[NSMenuItem separatorItem]];

   title = [@"Hide " stringByAppendingString:appName];
   [appleMenu addItemWithTitle:title action:@selector(hide:)
                 keyEquivalent:@"h"];

   menuItem = (NSMenuItem *)[appleMenu addItemWithTitle:@"Hide Others"
                                                 action:@selector(hideOtherApplications:)
                                          keyEquivalent:@"h"];
   [menuItem setKeyEquivalentModifierMask:(NSAlternateKeyMask|NSCommandKeyMask)];

   [appleMenu addItemWithTitle:@"Show All"
                        action:@selector(unhideAllApplications:)
                 keyEquivalent:@""];

   [appleMenu addItem:[NSMenuItem separatorItem]];

   title = [@"Quit " stringByAppendingString:appName];
   [appleMenu addItemWithTitle:title action:@selector(terminate:) keyEquivalent:@"q"];

   /* Put menu into the menubar */
   menuItem = [[NSMenuItem alloc] initWithTitle:@"" action:nil keyEquivalent:@""];
   [menuItem setSubmenu:appleMenu];
   [[NSApp mainMenu] addItem:menuItem];

   /* Tell the application object that this is now the application menu */
   [NSApp setAppleMenu:appleMenu];

   /* Finally give up our references to the objects */
   [appleMenu release];
   [menuItem release];
}

void ray_osx_setup_window_menu() {
   NSMenu      *windowMenu;
   NSMenuItem  *windowMenuItem;
   NSMenuItem  *menuItem;

   windowMenu = [[NSMenu alloc] initWithTitle:@"Window"];

   /* "Minimize" item */
   menuItem = [[NSMenuItem alloc] initWithTitle:@"Minimize"
                                         action:@selector(performMiniaturize:)
                                  keyEquivalent:@"m"];
   [windowMenu addItem:menuItem];
   [menuItem release];

   /* Put menu into the menubar */
   windowMenuItem = [[NSMenuItem alloc] initWithTitle:@"Window" action:nil
                                        keyEquivalent:@""];
   [windowMenuItem setSubmenu:windowMenu];
   [[NSApp mainMenu] addItem:windowMenuItem];

   /* Tell the application object that this is now the window menu */
   [NSApp setWindowsMenu:windowMenu];

   /* Finally give up our references to the objects */
   [windowMenu release];
   [windowMenuItem release];
}

void ray_osx_init() {
   if (ray_pool)
      return;

   ray_pool = [[NSAutoreleasePool alloc] init];
   [RayApplication sharedApplication];

   [NSApp setMainMenu:[[[NSMenu alloc] init] autorelease]];
   ray_osx_setup_application_menu();
   ray_osx_setup_window_menu();

   [NSApp finishLaunching];
   [NSApp updateWindows];
   [NSApp activateIgnoringOtherApps:true];
}

void ray_osx_close() {
   [ray_pool drain];
   ray_pool = nil;
}
