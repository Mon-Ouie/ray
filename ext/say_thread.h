#ifndef SAY_THREAD_H_
#define SAY_THREAD_H_

#include "say_basic_type.h"

typedef void *(*say_thread_func)(void *data);

#ifdef SAY_WIN
typedef struct {
  DWORD key;
} say_thread_variable;

typedef struct {
  HANDLE th;
  say_thread_func func;
  void *data;
  } say_thread;
#else
typedef struct {
  pthread_key_t key;
} say_thread_variable;

typedef struct {
  pthread_t th;
} say_thread;
#endif

say_thread_variable *say_thread_variable_create(say_destructor destructor);
void say_thread_variable_free(say_thread_variable *var);

void say_thread_variable_set(say_thread_variable *var, void *val);
void *say_thread_variable_get(say_thread_variable *var);

say_thread *say_thread_create(void *data, say_thread_func func);
void say_thread_free(say_thread *th);

void say_thread_join(say_thread *th);

#endif
