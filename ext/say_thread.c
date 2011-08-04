#include "say.h"

#ifdef SAY_WIN
/* Windows threads */

static unsigned int say_thread_entry_point(void *data) {
  say_thread *th = (say_thread*)data;
  th->func(th->data);
  _endthreadex(0);

  return 0;
}

say_thread *say_thread_create(void *data, say_thread_func func) {
  say_thread *th = malloc(sizeof(say_thread));

  th->func = func;
  th->data = data;

  th->th = _beginthreadex(NULL, 0, say_thread_entry_point, th, 0, NULL);

  return th;
}

void say_thread_free(say_thread *th) {
  CloseHandle(th->th);
  free(th);
}

void say_thread_join(say_thread *th) {
  WaitForSingleObject(th->th, INFINITE);
}

#else

/* POSIX threads */

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

#endif
