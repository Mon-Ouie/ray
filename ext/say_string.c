#include "say.h"

char *say_strdup(const char *str) {
  size_t size = sizeof(char) * (strlen(str) + 1);
  char *cpy = malloc(size);
  memcpy(cpy, str, size);

  return cpy;
}
