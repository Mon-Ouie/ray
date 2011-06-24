#include "say.h"

static GLuint say_current_buffer = 0;
static say_context *say_buffer_last_context = NULL;

static void say_buffer_make_current(GLuint vbo) {
  say_context *context = say_context_current();

  if (context != say_buffer_last_context ||
      vbo != say_current_buffer) {
    say_current_buffer = vbo;
    say_buffer_last_context = context;

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);
  }
}

static GLuint say_current_vao = 0;
static say_context *say_vao_last_context = NULL;

typedef struct {
  GLuint vao;
  say_context *context;
} say_vao_pair;

static void say_vao_make_current(GLuint vao) {
  say_context *context = say_context_current();

  if (context != say_vao_last_context ||
      vao != say_current_vao) {
    say_current_vao = vao;
    say_vao_last_context = context;

    glBindVertexArray(vao);
  }
}

static void say_buffer_will_delete(GLuint vbo) {
  if (vbo == say_current_buffer)
    say_current_buffer = 0;
}

static void say_buffer_delete_vao_pair(say_vao_pair *pair) {
  /* TODO: finding out if the context is still alive, to avoid leaks */
  if (say_vao_last_context == pair->context && say_current_vao == pair->vao) {
    say_current_vao = 0;
    glDeleteVertexArrays(1, &pair->vao);
  }

  free(pair);
}

static char *say_buffer_offset(size_t id) {
  return ((char*)NULL + id);
}

static char *say_buffer_pos_offset() {
  return say_buffer_offset(0);
}

static char *say_buffer_color_offset() {
  return say_buffer_offset(sizeof(float) * 2);
}

static char *say_buffer_tex_pos_offset() {
  return say_buffer_offset(sizeof(float) * 2 + sizeof(uint8_t) * 4);
}

static void say_buffer_build_vao(say_buffer *buf, GLuint vao) {
  say_vao_make_current(vao);
  say_buffer_make_current(buf->vbo);

  glVertexAttribPointerARB(SAY_POS_ID, 2, GL_FLOAT, GL_FALSE,
                           sizeof(say_vertex), say_buffer_pos_offset());

  glVertexAttribPointerARB(SAY_COLOR_ID, 4, GL_UNSIGNED_BYTE, GL_TRUE,
                           sizeof(say_vertex), say_buffer_color_offset());

  glVertexAttribPointerARB(SAY_TEX_COORD_ID, 2, GL_FLOAT, GL_FALSE,
                           sizeof(say_vertex), say_buffer_tex_pos_offset());

  for (int i = 0; i < SAY_ATTR_COUNT; i++)
    glEnableVertexAttribArrayARB(i);
}

static GLuint say_buffer_get_vao(say_buffer *buf) {
  say_context *ctxt = say_context_current();
  uint32_t count = ctxt->count;

  say_vao_pair *pair = say_table_get(buf->vaos, count);

  if (!pair) {
    pair = malloc(sizeof(say_vao_pair));
    say_table_set(buf->vaos, count, pair);

    pair->context = ctxt;

    glGenVertexArrays(1, &pair->vao);
    say_buffer_build_vao(buf, pair->vao);

    return pair->vao;
  }
  else {
    return pair->vao;
  }
}

say_buffer *say_buffer_create(GLenum type, size_t size) {
  say_context_ensure();

  say_buffer *buf = (say_buffer*)malloc(sizeof(say_buffer));
  buf->vaos = say_table_create((say_destructor)say_buffer_delete_vao_pair);

  glGenBuffersARB(1, &buf->vbo);
  say_buffer_make_current(buf->vbo);

  buf->type = type;

  buf->size = size;
  buf->buffer = (say_vertex*)malloc(sizeof(say_vertex) * size);

  glBufferDataARB(GL_ARRAY_BUFFER_ARB, buf->size * sizeof(say_vertex), NULL,
                  type);

  return buf;
}

void say_buffer_free(say_buffer *buf) {
  say_context_ensure();

  say_table_free(buf->vaos);

  say_buffer_will_delete(buf->vbo);
  glDeleteBuffersARB(1, &(buf->vbo));

  free(buf->buffer);
  free(buf);
}

say_vertex *say_buffer_get_vertex(say_buffer *buf, size_t id) {
  return &(buf->buffer[id]);
}

void say_buffer_bind(say_buffer *buf) {
  say_context_ensure();
  say_vao_make_current(say_buffer_get_vao(buf));
}

void say_buffer_unbind() {
  say_buffer_make_current(0);
  say_vao_make_current(0);
}

void say_buffer_update_part(say_buffer *buf, size_t id, size_t size) {
  say_context_ensure();

  if (size == 0) return;

  say_buffer_make_current(buf->vbo);
  glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, sizeof(say_vertex) * id,
                     sizeof(say_vertex) * size, &(buf->buffer[id]));
}

void say_buffer_update(say_buffer *buf) {
  say_buffer_update_part(buf, 0, buf->size);
}

size_t say_buffer_get_size(say_buffer *buf) {
  return buf->size;
}

void say_buffer_resize(say_buffer *buf, size_t size) {
  buf->size = size;
  buf->buffer = realloc(buf->buffer, sizeof(say_vertex) * size);

  say_buffer_make_current(buf->vbo);
  glBufferDataARB(GL_ARRAY_BUFFER_ARB, buf->size * sizeof(say_vertex),
                  buf->buffer, buf->type);
}
