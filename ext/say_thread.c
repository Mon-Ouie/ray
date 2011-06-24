#include "say.h"

say_thread *say_thread_create(void *data, say_thread_func func) {
  say_thread *th = malloc(sizeof(say_thread));
  pthread_create(&th->th, NULL, func, data);

  return th;
}

void say_thread_free(say_thread *th) {
  free(th);
}

void say_thread_join(say_thread *th) {
  pthread_join(th->th, NULL);
}
