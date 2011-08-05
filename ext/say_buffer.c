#include "say.h"

static bool say_has_vao() {
  return __GLEW_ARB_vertex_array_object ||
    __GLEW_APPLE_vertex_array_object;
}

static GLuint say_current_vbo = 0;
static say_context *say_vbo_last_context = NULL;

static void say_vbo_make_current(GLuint vbo) {
  say_context *context = say_context_current();

  if (context != say_vbo_last_context ||
      vbo != say_current_vbo) {
    say_current_vbo      = vbo;
    say_vbo_last_context = context;

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
    say_current_vao      = vao;
    say_vao_last_context = context;

    glBindVertexArray(vao);

    say_index_buffer_rebind();
  }
}

static say_buffer  *say_current_buffer      = NULL;
static say_context *say_buffer_last_context = NULL;

static void say_buffer_setup_pointer(say_buffer *buf);

static void say_buffer_make_current(say_buffer *buf) {
  say_context *context = say_context_current();

  if (context != say_buffer_last_context ||
      buf != say_current_buffer) {
    say_current_buffer      = buf;
    say_buffer_last_context = context;

    say_buffer_setup_pointer(buf);
  }
}

static void say_vbo_will_delete(GLuint vbo) {
  if (vbo == say_current_vbo)
    say_current_vbo = 0;
}

static void say_buffer_will_delete(say_buffer *buf) {
  if (buf == say_current_buffer)
    say_current_buffer = NULL;
}

static void say_buffer_delete_vao_pair(say_vao_pair *pair) {
  /* TODO: finding out if the context is still alive, to avoid leaks */
  if (say_vao_last_context == pair->context && say_current_vao == pair->vao) {
    say_current_vao = 0;
    glDeleteVertexArrays(1, &pair->vao);

    say_index_buffer_rebind();
  }

  free(pair);
}

static size_t say_buffer_register_pointer(size_t attr_i, say_vertex_elem_type t,
                                          size_t stride, size_t offset) {
  switch (t) {
  case SAY_FLOAT:
    glVertexAttribPointerARB(attr_i, 1, GL_FLOAT, GL_FALSE, stride,
                             (void*)offset);
    return offset + sizeof(GLfloat);
  case SAY_INT:
    glVertexAttribPointerARB(attr_i, 1, GL_INT, GL_FALSE, stride,
                             (void*)offset);
    return offset + sizeof(GLint);
  case SAY_UBYTE:
    glVertexAttribPointerARB(attr_i, 1, GL_UNSIGNED_BYTE, GL_FALSE, stride,
                             (void*)offset);
    return offset + sizeof(GLubyte);
  case SAY_BOOL:
    glVertexAttribPointerARB(attr_i, 1, GL_INT, GL_FALSE, stride,
                             (void*)offset);
    return offset + sizeof(GLint);

  case SAY_COLOR:
    glVertexAttribPointerARB(attr_i, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride,
                             (void*)offset);
    return offset + sizeof(GLubyte) * 4;
  case SAY_VECTOR2:
    glVertexAttribPointerARB(attr_i, 2, GL_FLOAT, GL_FALSE, stride,
                             (void*)offset);
    return offset + sizeof(GLfloat) * 2;
  case SAY_VECTOR3:
    glVertexAttribPointerARB(attr_i, 3, GL_FLOAT, GL_FALSE, stride,
                             (void*)offset);
    return offset + sizeof(GLfloat) * 3;
  }

  return offset;
}

static void say_buffer_setup_pointer(say_buffer *buf) {
  say_vbo_make_current(buf->vbo);

  say_vertex_type *type = say_get_vertex_type(buf->vtype);

  size_t count = say_vertex_type_get_elem_count(type);

  size_t stride = say_vertex_type_get_size(type);
  size_t offset = 0;

  size_t instance_stride = say_vertex_type_get_instance_size(type);
  size_t instance_offset = 0;

  size_t i = 0;
  for (; i < count; i++) {
    say_vertex_elem_type t = say_vertex_type_get_type(type, i);

    if (say_vertex_type_is_per_instance(type, i)) {
      say_vbo_make_current(buf->instance_vbo);
      instance_offset = say_buffer_register_pointer(i, t, instance_stride,
                                                    instance_offset);
    }
    else {
      say_vbo_make_current(buf->vbo);
      offset = say_buffer_register_pointer(i, t, stride, offset);
    }

    glEnableVertexAttribArrayARB(i);
    if (glVertexAttribDivisorARB) {
      if (say_vertex_type_is_per_instance(type, i))
        glVertexAttribDivisorARB(i, 1);
      else
        glVertexAttribDivisorARB(i, 0);
    }
  }

  /*
   * Say will always use all the attribs. Disable all of them until
   * finding one that is already disabled.
   */
  for (; i < GL_MAX_VERTEX_ATTRIBS_ARB; i++) {
    GLint enabled;
    glGetVertexAttribivARB(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED_ARB, &enabled);

    if (enabled)
      glDisableVertexAttribArrayARB(i);
    else
      break;
  }
}

static void say_buffer_build_vao(say_buffer *buf, GLuint vao) {
  say_vao_make_current(vao);
  say_buffer_setup_pointer(buf);
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

say_buffer *say_buffer_create(size_t vtype, GLenum type, size_t size) {
  say_context_ensure();

  say_buffer *buf = (say_buffer*)malloc(sizeof(say_buffer));

  if (say_has_vao())
    buf->vaos = say_table_create((say_destructor)say_buffer_delete_vao_pair);
  else
    buf->vaos = NULL;

  buf->vtype = vtype;

  glGenBuffersARB(1, &buf->vbo);
  say_vbo_make_current(buf->vbo);

  buf->type = type;

  say_vertex_type *vtype_ref = say_get_vertex_type(vtype);
  size_t byte_size = say_vertex_type_get_size(vtype_ref);
  buf->buffer = say_array_create(byte_size, NULL, NULL);
  say_array_resize(buf->buffer, size);

  glBufferDataARB(GL_ARRAY_BUFFER_ARB, size * byte_size, NULL, type);

  buf->instance_vbo    = 0;
  buf->instance_buffer = NULL;

  if (say_vertex_type_has_instance_data(vtype_ref)) {
    glGenBuffersARB(1, &buf->instance_vbo);
    say_vbo_make_current(buf->instance_vbo);

    byte_size = say_vertex_type_get_instance_size(vtype_ref);
    buf->instance_buffer = say_array_create(byte_size, NULL, NULL);
  }

  return buf;
}

void say_buffer_free(say_buffer *buf) {
  say_context_ensure();

  if (buf->vaos)
    say_table_free(buf->vaos);
  else
    say_buffer_will_delete(buf);

  say_vbo_will_delete(buf->vbo);
  glDeleteBuffersARB(1, &buf->vbo);

  if (buf->instance_vbo) {
    say_vbo_will_delete(buf->instance_vbo);
    glDeleteBuffersARB(1, &buf->instance_vbo);

    say_array_free(buf->instance_buffer);
  }

  say_array_free(buf->buffer);
  free(buf);
}

bool say_buffer_has_instance(say_buffer *buf) {
  return buf->instance_buffer != NULL;
}

void *say_buffer_get_vertex(say_buffer *buf, size_t id) {
  return say_array_get(buf->buffer, id);
}

void *say_buffer_get_instance(say_buffer *buf, size_t id) {
  return say_array_get(buf->instance_buffer, id);
}

void say_buffer_bind(say_buffer *buf) {
  say_context_ensure();

  if (say_has_vao())
    say_vao_make_current(say_buffer_get_vao(buf));
  else
    say_buffer_make_current(buf);
}

void say_buffer_bind_vbo(say_buffer *buf) {
  say_context_ensure();
  say_vbo_make_current(buf->vbo);
}

void say_buffer_bind_instance_vbo(say_buffer *buf) {
  say_context_ensure();
  say_vbo_make_current(buf->instance_vbo);
}

void say_buffer_unbind() {
  say_context_ensure();

  if (say_has_vao())
    say_vao_make_current(0);
  else { /* disable vertex attribs */
    for (size_t i = 0; i < GL_MAX_VERTEX_ATTRIBS_ARB; i++) {
      GLint enabled;
      glGetVertexAttribivARB(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED_ARB, &enabled);

      if (enabled)
        glDisableVertexAttribArrayARB(i);
      else
        break;
    }
  }
}

void say_buffer_unbind_vbo() {
  say_context_ensure();
  say_vbo_make_current(0);
}

void say_buffer_update_part(say_buffer *buf, size_t id, size_t size) {
  if (size == 0) return;

  say_context_ensure();

  size_t byte_size = say_array_get_elem_size(buf->buffer);
  say_vbo_make_current(buf->vbo);
  glBufferSubDataARB(GL_ARRAY_BUFFER_ARB,
                     byte_size * id,
                     byte_size * size,
                     say_buffer_get_vertex(buf, id));
}

void say_buffer_update(say_buffer *buf) {
  say_buffer_update_part(buf, 0, say_array_get_size(buf->buffer));
}

size_t say_buffer_get_size(say_buffer *buf) {
  return say_array_get_size(buf->buffer);
}

void say_buffer_resize(say_buffer *buf, size_t size) {
  say_array_resize(buf->buffer, size);

  say_context_ensure();
  say_vbo_make_current(buf->vbo);
  glBufferDataARB(GL_ARRAY_BUFFER_ARB,
                  size * say_array_get_elem_size(buf->buffer),
                  say_buffer_get_vertex(buf, 0),
                  buf->type);
}

void say_buffer_update_instance_part(say_buffer *buf, size_t id,
                                     size_t size) {
  if (size == 0) return;

  say_context_ensure();

  size_t byte_size = say_array_get_elem_size(buf->instance_buffer);
  say_vbo_make_current(buf->instance_vbo);
  glBufferSubDataARB(GL_ARRAY_BUFFER_ARB,
                     byte_size * id,
                     byte_size * size,
                     say_buffer_get_instance(buf, id));
}

void say_buffer_update_instance(say_buffer *buf) {
  say_buffer_update_instance_part(buf, 0,
                                  say_array_get_size(buf->instance_buffer));
}

size_t say_buffer_get_instance_size(say_buffer *buf) {
  return say_array_get_size(buf->instance_buffer);
}

void say_buffer_resize_instance(say_buffer *buf, size_t size) {
  say_array_resize(buf->instance_buffer, size);

  say_context_ensure();
  say_vbo_make_current(buf->instance_vbo);
  glBufferDataARB(GL_ARRAY_BUFFER_ARB,
                  size * say_array_get_elem_size(buf->instance_buffer),
                  say_buffer_get_instance(buf, 0),
                  buf->type);
}
