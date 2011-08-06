/*
 * Code stolen from GLEW, to do what GLEW should be doing.
 */
#include "say.h"

#ifdef SAY_WIN
void *say_get_proc(const char *name) {
  return (void*)wglGetProcAddress((LPCSTR)name);
}
#else
# ifdef SAY_OSX

#include <mach-o/dyld.h>
void *say_get_proc(const char *name) {
  static const struct mach_header *image = NULL;
  NSSymbol symbol;
  char*    symbol_name;

  if (!image) {
    image = NSAddImage("/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL",
                         NSADDIMAGE_OPTION_RETURN_ON_ERROR);
  }

  /* prepend a '_' for the Unix C symbol mangling convention */
  symbol_name = malloc(strlen((const char*)name) + 2);
  strcpy(symbol_name + 1, (const char*)name);
  symbol_name[0] = '_';
  symbol = NULL;

  symbol = image ? NSLookupSymbolInImage(image, symbol_name,
                                           NSLOOKUPSYMBOLINIMAGE_OPTION_BIND |
                                           NSLOOKUPSYMBOLINIMAGE_OPTION_RETURN_ON_ERROR) :
    NULL;
  free(symbol_name);

  if (symbol) return NSAddressOfSymbol(symbol);
  return NULL;
}
# else
void *say_get_proc(const char *name) {
  return (void*)glXGetProcAddress((GLubyte*)name);
}
# endif
#endif
