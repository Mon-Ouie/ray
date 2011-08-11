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

#include <dlfcn.h>
void *say_get_proc(const char *name) {
  static void *handle = NULL;
  if (!handle) {
    handle = dlopen("/System/Library/Frameworks/OpenGL.framework/"
                    "Versions/Current/OpenGL", RTLD_LAZY);
  }

  /* prepend a '_' for the Unix C symbol mangling convention */
  char *symbol_name = malloc(strlen(name) + 2);
  strcpy(symbol_name + 1, name);
  symbol_name[0] = '_';

  return dlsym(handle, symbol_name);
}
# else
void *say_get_proc(const char *name) {
  return (void*)glXGetProcAddress((GLubyte*)name);
}
# endif
#endif
