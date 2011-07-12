#include "say.h"

#ifdef SAY_WIN
say_thread_variable *say_thread_variable_create(say_destructor destructor) {
  say_thread_variable *var = malloc(sizeof(say_thread_variable));
  var->key = TlsAlloc();
  
  return var;
}

void say_thread_variable_free(say_thread_variable *var) {
  TlsFree(var->key);
  free(var);
}

void say_thread_variable_set(say_thread_variable *var, void *val) {
  TlsSetValue(var->key, val);
}

void *say_thread_variable_get(say_thread_variable *var) {
  return TlsGetValue(var->key);
}
#else
say_thread_variable *say_thread_variable_create(say_destructor destructor) {
  say_thread_variable *var = malloc(sizeof(say_thread_variable));

  pthread_key_create(&(var->key), destructor);

  return var;
}

void say_thread_variable_free(say_thread_variable *var) {
  pthread_key_delete(var->key);
  free(var);
}

void say_thread_variable_set(say_thread_variable *var, void *val) {
  pthread_setspecific(var->key, val);
}

void *say_thread_variable_get(say_thread_variable *var) {
  return pthread_getspecific(var->key);
}
#endif