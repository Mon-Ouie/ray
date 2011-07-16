#include "ray.h"

VALUE ray_cGLIndexBuffer = Qnil;

say_index_buffer *ray_rb2index_buffer(VALUE obj) {
  if (!RAY_IS_A(obj, rb_path2class("Ray::GL::IndexBuffer"))) {
    rb_raise(rb_eTypeError, "Can't convert %s into Ray::GL::IndexBuffer",
             RAY_OBJ_CLASSNAME(obj));
  }

  say_index_buffer **ptr = NULL;
  Data_Get_Struct(obj, say_index_buffer*, ptr);

  if (!*ptr)
    rb_raise(rb_eRuntimeError, "trying to use un-initialized index buffer");

  return *ptr;
}

static
void ray_index_buffer_free(say_index_buffer **ptr) {
  if (*ptr)
    say_index_buffer_free(*ptr);
}

static
VALUE ray_gl_index_buffer_alloc(VALUE self) {
  say_index_buffer **buf = malloc(sizeof(say_index_buffer*));
  return Data_Wrap_Struct(self, NULL, ray_index_buffer_free, buf);
}


/*
 * @overload initialize(type)
 *   @param type (see Ray::BufferRenderer#initialize)
 *   Creates a new index buffer with an arbitrary small size (256).
 */
static
VALUE ray_gl_index_buffer_init(VALUE self, VALUE type) {
  say_index_buffer **ptr = NULL;
  Data_Get_Struct(self, say_index_buffer*, ptr);

  *ptr = say_index_buffer_create(ray_buf_type(type), 256);

  return self;
}


/* Unbinds any buffer that was bound. */
static
VALUE ray_gl_index_buffer_unbind(VALUE self) {
  say_index_buffer_unbind();
  return Qnil;
}

/* Binds the receiver */
static
VALUE ray_gl_index_buffer_bind(VALUE self) {
  say_index_buffer_bind(ray_rb2index_buffer(self));
  return self;
}

/*
 * @overload [](id)
 *   @param [Integer] id
 *   @return [Index] The indexed stored at that index
 */
static
VALUE ray_gl_index_buffer_get(VALUE self, VALUE i) {
  say_index_buffer *buf   = ray_rb2index_buffer(self);
  size_t            size  = say_index_buffer_get_size(buf);
  size_t            index = NUM2ULONG(i);

  if (index >= size)
    return Qnil;

  return ULONG2NUM(*say_index_buffer_get(buf, index));
}

/*
 * @overload []=(id, value)
 *   @param [Integer] id
 *   @param [Integer] value The index to store in the buffer
 */
static
VALUE ray_gl_index_buffer_set(VALUE self, VALUE i, VALUE val) {
  rb_check_frozen(self);

  say_index_buffer *buf   = ray_rb2index_buffer(self);
  size_t            index = NUM2ULONG(i);
  size_t            size  = say_index_buffer_get_size(buf);

  if (index >= size) {
    rb_raise(rb_eRangeError, "%zu is outside of range 0...%zu",
             size, index);
  }

  *say_index_buffer_get(buf, index) = NUM2ULONG(val);

  return val;
}

/*
 * @overload update(range = 0...size)
 *   @param [Range<Integer>] range Indices of indices to update
 *
 *   Updates a part of the buffer.
 *
 * @overload update(first, size)
 *   @param [Integer] first First index to update
 *   @param [Integer] size Amount of indices to update
 */
static
VALUE ray_gl_index_buffer_update(int argc, VALUE *argv, VALUE self) {
  say_index_buffer *buf       = ray_rb2index_buffer(self);
  size_t            max_index = say_index_buffer_get_size(buf);

  if (argc == 0)
    say_index_buffer_update(buf);
  else if (argc == 2) {
    size_t begin = NUM2ULONG(argv[0]);
    size_t end   = NUM2ULONG(argv[1]);

    if (end > max_index)
      end = max_index;

    if (begin > end || begin > max_index)
      return self;

    size_t size = (end - begin) + 1;

    say_index_buffer_update_part(buf, begin, size);
  }
  else {
    VALUE range;
    rb_scan_args(argc, argv, "1", &range); /* raise exception */

    size_t begin = NUM2ULONG(rb_funcall(range, RAY_METH("begin"), 0));
    size_t end   = NUM2ULONG(rb_funcall(range, RAY_METH("end"), 0));

    if (end > max_index)
      end = max_index;

    if (begin > end || begin > max_index)
      return self;

    size_t size = (end - begin) + 1;

    say_index_buffer_update_part(buf, begin, size);
  }

  return self;
}

/* @return [Integer] Size of the buffer (amount of indices it contains) */
static
VALUE ray_gl_index_buffer_size(VALUE self) {
  return ULONG2NUM(say_index_buffer_get_size(ray_rb2index_buffer(self)));
}

/*
 * @overload resize(size)
 *   @param [Integere] size New size
 *   Resizes the index buffer, alsos causing it to be updated.
 */
static
VALUE ray_gl_index_buffer_resize(VALUE self, VALUE size) {
  rb_check_frozen(self);
  say_index_buffer_resize(ray_rb2index_buffer(self), NUM2ULONG(size));
  return self;
}

/*
 * Document-class: Ray::GL::IndexBuffer
 *
 * Index buffers are very similar to Ray::GL::Buffer. They are buffers used to
 * contain indices, which allows to avoid sending the same vertex twice when
 * drawing.
 *
 */
void Init_ray_gl_index_buffer() {
  ray_cGLIndexBuffer = rb_define_class_under(ray_mGL, "IndexBuffer",
                                             rb_cObject);

  rb_define_alloc_func(ray_cGLIndexBuffer, ray_gl_index_buffer_alloc);
  rb_define_method(ray_cGLIndexBuffer, "initialize", ray_gl_index_buffer_init,
                   1);

  rb_define_singleton_method(ray_cGLIndexBuffer, "unbind",
                             ray_gl_index_buffer_unbind, 0);
  rb_define_method(ray_cGLIndexBuffer, "bind", ray_gl_index_buffer_bind, 0);

  rb_define_method(ray_cGLIndexBuffer, "[]",  ray_gl_index_buffer_get, 1);
  rb_define_method(ray_cGLIndexBuffer, "[]=", ray_gl_index_buffer_set, 2);

  rb_define_method(ray_cGLIndexBuffer, "update", ray_gl_index_buffer_update,
                   -1);

  rb_define_method(ray_cGLIndexBuffer, "size", ray_gl_index_buffer_size, 0);
  rb_define_method(ray_cGLIndexBuffer, "resize", ray_gl_index_buffer_resize,
                   1);
}
