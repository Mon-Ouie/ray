#ifndef SAY_CONTEXT_H_
#define SAY_CONTEXT_H_

#include "say_basic_type.h"

struct say_window;

typedef struct {
  uint32_t count;

#ifdef SAY_OSX
  SayContext *context;
#else
  GLXContext context;

  Display *dis;
  Window win;

  uint8_t should_free_window;
#endif
} say_context;

void say_context_ensure();

say_context *say_context_current();

say_context *say_context_create_for_window(struct say_window *window);
say_context *say_context_create();
void say_context_free(say_context *context);

void say_context_make_current(say_context *context);
void say_context_update(say_context *context);

void say_context_clean_up();

#endif
