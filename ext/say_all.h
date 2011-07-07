#ifndef SAY_ALL_H_
#define SAY_ALL_H_

/* Dependencies for all of the files */

/* Standard headers */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>

/* Threading */
#include <pthread.h>

/* OpenGL */
#include <GL/glew.h>

/* Fonts */
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_BITMAP_H

/* Audio */
#ifdef SAY_OSX
# include <OpenAL/al.h>
# include <OpenAL/alc.h>
#else
# include <AL/al.h>
# include <AL/alc.h>
#endif

#include <sndfile.h>

#ifdef HAVE_X11_EXTENSIONS_XRANDR_H
# define HAVE_XRANDR 1
#endif

/* Windowing */

#ifdef SAY_OSX
# include "say_osx.h"
#else
# include <X11/Xlib.h>
# include <X11/Xatom.h>
# ifdef HAVE_XRANDR
#  include <X11/extensions/Xrandr.h>
# endif
# include <GL/glx.h>
#endif

/* Clean up */
void say_clean_up();

/* String manipulations */
uint32_t say_utf8_to_utf32(const uint8_t *string);
char *say_strdup(const char *str);

/* Errors */

const char *say_error_get_last();
void say_error_set(const char *message);

void say_error_clean_up();

#define SAY_PI 3.14159265358979323846

#endif
