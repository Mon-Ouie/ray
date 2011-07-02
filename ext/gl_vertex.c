#include "ray.h"

VALUE ray_cGLVertex = Qnil;
static VALUE ray_gl_vertex_types = Qnil;

VALUE ray_get_vertex_class(size_t id) {
  VALUE vclass = rb_path2class("Ray::GL::Vertex");
  return rb_hash_aref(rb_iv_get(vclass, "@vertex_classes"), INT2FIX(id));
}

size_t ray_get_vtype(VALUE class) {
  size_t id = NUM2ULONG(rb_iv_get(class, "@vertex_type_id"));

  if (ray_get_vertex_class(id) == class)
    return id;
  else {
    rb_raise(rb_eRuntimeError, "expected class to be a vertex class");
    return 0;
  }
}

VALUE ray_gl_vertex_alloc(VALUE self) {
  size_t size = NUM2ULONG(rb_iv_get(self, "@vertex_type_size"));

  void *vertex = malloc(size);
  return Data_Wrap_Struct(self, NULL, free, vertex);
}

static
VALUE ray_gl_vertex_make_type(VALUE self, VALUE types) {
  size_t size = RARRAY_LEN(types);
  if (size == 0) {
    rb_raise(rb_eArgError, "can't create empty vertex type");
  }

  size_t vtype_id = say_vertex_type_make_new();
  say_vertex_type *vtype = say_get_vertex_type(vtype_id);

  for (size_t i = 0; i < size; i++) {
    VALUE element = RAY_ARRAY_AT(types, i);

    VALUE name = RAY_ARRAY_AT(element, 0);
    VALUE type = RAY_ARRAY_AT(element, 1);

    char *c_name = StringValuePtr(name);
    say_vertex_elem_type c_type = NUM2INT(rb_hash_aref(ray_gl_vertex_types,
                                                       type));

    say_vertex_elem c_elem = {c_type, c_name};
    say_vertex_type_push(vtype, c_elem);
  }

  return INT2FIX(vtype_id);
}

static
VALUE ray_gl_vertex_offset_of(VALUE self, VALUE vtype, VALUE elem_id) {
  say_vertex_type *type = say_get_vertex_type(NUM2ULONG(vtype));
  return INT2FIX(say_vertex_type_get_offset(type, NUM2INT(elem_id)));
}

static
VALUE ray_gl_vertex_size(VALUE self, VALUE vtype) {
  say_vertex_type *type = say_get_vertex_type(NUM2ULONG(vtype));
  return INT2FIX(say_vertex_type_get_size(type));
}

VALUE ray_gl_vertex_element(VALUE self, VALUE offset, VALUE type) {
  uint8_t *data = NULL;
  Data_Get_Struct(self, uint8_t, data);

  data += NUM2ULONG(offset);

  switch (NUM2INT(rb_hash_aref(ray_gl_vertex_types, type))) {
  case SAY_FLOAT:
    return rb_float_new(*(GLfloat*)data);
  case SAY_INT:
    return INT2FIX((*(GLint*)data));
  case SAY_UBYTE:
    return INT2FIX((*(GLubyte*)data));
  case SAY_BOOL:
    return (*(GLint*)data) ? Qtrue : Qfalse;

  case SAY_COLOR:
    return ray_col2rb(*(say_color*)data);
  case SAY_VECTOR2:
      return ray_vector2_to_rb(*(say_vector2*)data);
  case SAY_VECTOR3:
    return ray_vector3_to_rb(*(say_vector3*)data);
  }

  return Qnil;
}

VALUE ray_gl_vertex_set_element(VALUE self, VALUE offset, VALUE type,
                                VALUE val) {
  uint8_t *data = NULL;
  Data_Get_Struct(self, uint8_t, data);

  data += NUM2ULONG(offset);

  switch (NUM2INT(rb_hash_aref(ray_gl_vertex_types, type))) {
  case SAY_FLOAT:
    (*(GLfloat*)data) = NUM2DBL(val);
    break;
  case SAY_INT:
    ((*(GLint*)data)) = NUM2INT(val);
    break;
  case SAY_UBYTE:
    ((*(GLubyte*)data)) = ray_byte_clamp(NUM2INT(val));
    break;
  case SAY_BOOL:
    (*(GLint*)data) = RTEST(val);
    break;

  case SAY_COLOR:
    ((*(say_color*)data)) = ray_rb2col(val);
    break;
  case SAY_VECTOR2:
    ((*(say_vector2*)data)) = ray_convert_to_vector2(val);
    break;
  case SAY_VECTOR3:
    ((*(say_vector3*)data)) = ray_convert_to_vector3(val);
    break;
  }

  return type;
}

void Init_ray_gl_vertex() {
  ray_cGLVertex = rb_define_class_under(ray_mGL, "Vertex", rb_cObject);
  rb_define_alloc_func(ray_cGLVertex, ray_gl_vertex_alloc);

  rb_define_singleton_method(ray_cGLVertex, "make_type",
                             ray_gl_vertex_make_type, 1);
  rb_define_singleton_method(ray_cGLVertex, "offset_of",
                             ray_gl_vertex_offset_of, 2);
  rb_define_singleton_method(ray_cGLVertex, "size",
                             ray_gl_vertex_size, 1);

  rb_define_private_method(ray_cGLVertex, "element", ray_gl_vertex_element, 2);
  rb_define_private_method(ray_cGLVertex, "set_element",
                           ray_gl_vertex_set_element, 3);

  ray_gl_vertex_types = rb_hash_new();

  rb_hash_aset(ray_gl_vertex_types, RAY_SYM("float"), INT2FIX(SAY_FLOAT));
  rb_hash_aset(ray_gl_vertex_types, RAY_SYM("int"),   INT2FIX(SAY_INT));
  rb_hash_aset(ray_gl_vertex_types, RAY_SYM("ubyte"), INT2FIX(SAY_UBYTE));
  rb_hash_aset(ray_gl_vertex_types, RAY_SYM("bool"),  INT2FIX(SAY_BOOL));

  rb_hash_aset(ray_gl_vertex_types, RAY_SYM("color"),   INT2FIX(SAY_COLOR));
  rb_hash_aset(ray_gl_vertex_types, RAY_SYM("vector2"), INT2FIX(SAY_VECTOR2));
  rb_hash_aset(ray_gl_vertex_types, RAY_SYM("vector3"), INT2FIX(SAY_VECTOR3));

  rb_define_const(ray_cGLVertex, "TypeMap", ray_gl_vertex_types);
}
