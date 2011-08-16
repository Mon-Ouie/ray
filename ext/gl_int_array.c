#include "ray.h"

VALUE ray_cIntArray = Qnil;

mo_array *ray_rb2int_array(VALUE obj) {
  if (!rb_obj_is_kind_of(obj, rb_path2class("Ray::GL::IntArray"))) {
    rb_raise(rb_eTypeError, "can't convert %s into Ray::GL::IntArray",
             RAY_OBJ_CLASSNAME(obj));
  }

  mo_array *ptr = NULL;
  Data_Get_Struct(obj, mo_array, ptr);

  return ptr;
}

static
void ray_int_set_to_zero(void *elem) {
  *(GLint*)elem = 0;
}

static
VALUE ray_int_array_alloc(VALUE self) {
  mo_array *obj = mo_array_create(sizeof(GLint));
  obj->init = ray_int_set_to_zero;

  return Data_Wrap_Struct(self, NULL, mo_array_free, obj);
}

/*
  @overload initialize(*args)
    @param [Array<Integer>] args Inital content of the array
 */
static
VALUE ray_int_array_init(int argc, VALUE *argv, VALUE self) {
  mo_array *ary = ray_rb2int_array(self);

  for (int i = 0; i < argc; i++) {
    GLint val = NUM2INT(argv[i]);
    mo_array_push(ary, &val);
  }

  return self;
}

static
VALUE ray_int_array_init_copy(VALUE self, VALUE orig) {
  mo_array_copy(ray_rb2int_array(self), ray_rb2int_array(orig));
  return self;
}

/*
  @overload <<(val)
    @param [Integer] val Element to push
    @return [self]
*/
static
VALUE ray_int_array_push(VALUE self, VALUE val) {
  rb_check_frozen(self);

  mo_array *ary = ray_rb2int_array(self);
  GLint     e   = NUM2INT(val);

  mo_array_push(ary, &e);

  return self;
}

/*
  @overload [](i)
    @param [Integer] i Index to read from
    @return [Integer, nil] Value at that index
*/
static
VALUE ray_int_array_get(VALUE self, VALUE i) {
  mo_array *ary = ray_rb2int_array(self);
  size_t idx = NUM2ULONG(i);

  GLint *elem = mo_array_get_ptr(ary, idx, GLint);

  if (elem) {
    return INT2FIX(*elem);
  }
  else
    return Qnil;
}

/*
  @overload []=(i, val)
    @param [Integer] i Index of the value to change
    @param [Integer] val Value to assign
 */
static
VALUE ray_int_array_set(VALUE self, VALUE i, VALUE val) {
  rb_check_frozen(self);

  mo_array *ary  = ray_rb2int_array(self);
  size_t     idx = NUM2ULONG(i);

  if (ary->size <= idx)
    mo_array_resize(ary, idx + 1);

  mo_array_get_as(ary, idx, GLint) = NUM2INT(val);

  return val;
}

/* @return [Integer] size of the array */
static
VALUE ray_int_array_size(VALUE self) {
  return INT2FIX(ray_rb2int_array(self)->size);
}

/* Removes all the elements from the array */
static
VALUE ray_int_array_clear(VALUE self) {
  mo_array_resize(ray_rb2int_array(self), 0);
  return self;
}

/*
  DocumentClass: Ray::GL::IntArray

  Class used to send indices to OpenGL. It is not meant for any other use.
*/
void Init_ray_int_array() {
  ray_cIntArray = rb_define_class_under(ray_mGL, "IntArray", rb_cObject);
  rb_define_alloc_func(ray_cIntArray, ray_int_array_alloc);
  rb_define_method(ray_cIntArray, "initialize", ray_int_array_init, -1);
  rb_define_method(ray_cIntArray, "initialize_copy", ray_int_array_init_copy,
                   1);

  rb_define_method(ray_cIntArray, "<<", ray_int_array_push, 1);

  rb_define_method(ray_cIntArray, "[]", ray_int_array_get, 1);
  rb_define_method(ray_cIntArray, "[]=", ray_int_array_set, 2);

  rb_define_method(ray_cIntArray, "size", ray_int_array_size, 0);

  rb_define_method(ray_cIntArray, "clear", ray_int_array_clear, 0);
}
