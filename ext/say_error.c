#include "say.h"

static say_thread_variable *say_last_error = NULL;

const char *say_error_get_last() {
  if (!say_last_error)
    return NULL;

  return say_thread_variable_get(say_last_error);
}

static char *say_strdup(const char *str) {
  size_t size = sizeof(char) * (strlen(str) + 1);
  char *cpy = malloc(size);
  memcpy(cpy, str, size);

  return cpy;
}

void say_error_set(const char *message) {
  if (!say_last_error)
    say_last_error = say_thread_variable_create(free);

  void *old = say_thread_variable_get(say_last_error);
  if (old)
    free(old);

  say_thread_variable_set(say_last_error, say_strdup(message));
}
