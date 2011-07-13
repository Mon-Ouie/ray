#include "ray.h"

VALUE ray_cBufferRenderer = Qnil;

GLenum ray_buf_type(VALUE type) {
  if (type == RAY_SYM("static"))
    return SAY_STATIC;
  else if (type == RAY_SYM("stream"))
    return SAY_STREAM;
  else if (type == RAY_SYM("dynamic"))
    return SAY_DYNAMIC;
  else {
    rb_raise(rb_eRuntimeError, "unknown buffer type");
    return 0;
  }
}

say_buffer_renderer *ray_rb2buf_renderer(VALUE obj) {
  if (!RAY_IS_A(obj, rb_path2class("Ray::BufferRenderer"))) {
    rb_raise(rb_eTypeError, "Can't convert %s into Ray::BufferRenderer",
             RAY_OBJ_CLASSNAME(obj));
  }

  say_buffer_renderer **ptr;
  Data_Get_Struct(obj, say_buffer_renderer*, ptr);

  if (!*ptr) {
    rb_raise(rb_eRuntimeError, "trying to use an uninitialized renderer");
  }

  return *ptr;
}

static
void ray_buffer_renderer_free(say_buffer_renderer **ptr) {
  if (*ptr)
    say_buffer_renderer_free(*ptr);
}

static
VALUE ray_buffer_renderer_alloc(VALUE self) {
  say_buffer_renderer **ptr = malloc(sizeof(say_buffer_renderer*));
  return Data_Wrap_Struct(self, NULL, ray_buffer_renderer_free, ptr);
}

/*
 * @overload initialize(type, vtype)
 *   @param [Symbol] type Type of the buffer (:static, :stream, :dynamic).
 *   @param [Class] vtype Class of vertices.
 *
 *   Creates a new buffer renderer.
 *
 *   The type argument indicates how often you may need to update the
 *   buffer. Stream, working in most cases, means you are going to update the
 *   stream once and use it once. Static means you will just push your data once
 *   and use them many times. Dynamic, lastly, means your data will be both
 *   updated and drawn often.
 */
static
VALUE ray_buffer_renderer_init(VALUE self, VALUE type, VALUE vtype) {
  rb_iv_set(self, "@drawables", rb_ary_new());

  say_buffer_renderer **ptr;
  Data_Get_Struct(self, say_buffer_renderer*, ptr);

  *ptr = say_buffer_renderer_create(ray_buf_type(type),
                                    ray_get_vtype(vtype));

  return self;
}

/* Removes all the drawables from the renderer. */
static
VALUE ray_buffer_renderer_clear(VALUE self) {
  say_buffer_renderer_clear(ray_rb2buf_renderer(self));
  rb_ary_clear(rb_iv_get(self, "@drawables"));
  return self;
}

/*
 * @overload push(object)
 *   @param [Ray::Drawable] object
 *
 *   Adds a drawable to render. It won't be rendered correctly until you call
 *   update.
 */
static
VALUE ray_buffer_renderer_push(VALUE self, VALUE obj) {
  rb_check_frozen(self);

  if (!say_buffer_renderer_push(ray_rb2buf_renderer(self),
                                ray_rb2drawable(obj))) {
    rb_raise(rb_eRuntimeError, "%s", say_error_get_last());
  }

  rb_ary_push(rb_iv_get(self, "@drawables"), obj);

  return self;
}

/* Upadates the buffer */
static
VALUE ray_buffer_renderer_update(VALUE self) {
  say_buffer_renderer_update(ray_rb2buf_renderer(self));
  return self;
}

/*
 * Document-class: Ray::BufferRenderer
 *
 * Buffer renderers are a way to draw many drawable quickly, assuming they are
 * not updated too often. It makes things quite faster if you have many
 * drawables that won't need to be updated for a loong time.
 *
 * It simply uses drawables to fill its own buffer, and ask those drawables to
 * draw themselves from it.
 */
void Init_ray_buffer_renderer() {
  ray_cBufferRenderer = rb_define_class_under(ray_mRay, "BufferRenderer",
                                              rb_cObject);

  rb_define_alloc_func(ray_cBufferRenderer, ray_buffer_renderer_alloc);
  rb_define_method(ray_cBufferRenderer, "initialize", ray_buffer_renderer_init,
                   2);

  rb_define_method(ray_cBufferRenderer, "clear", ray_buffer_renderer_clear, 0);
  rb_define_method(ray_cBufferRenderer, "push", ray_buffer_renderer_push, 1);
  rb_define_method(ray_cBufferRenderer, "update", ray_buffer_renderer_update,
                   0);
}
