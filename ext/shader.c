#include "ray.h"

VALUE ray_cShader = Qnil;

VALUE ray_shader2rb(say_shader *shader, VALUE owner) {
  VALUE obj = Data_Wrap_Struct(rb_path2class("Ray::Shader"), NULL, NULL,
                               shader);
  rb_iv_set(obj, "@owner", owner);
  rb_iv_set(obj, "@locations", rb_hash_new());
  rb_iv_set(obj, "@images", rb_hash_new());
  return obj;
}

say_shader *ray_rb2shader(VALUE obj) {
  if (!RAY_IS_A(obj, rb_path2class("Ray::Shader"))) {
    rb_raise(rb_eTypeError, "Can't convert %s into Ray::Shader",
             RAY_OBJ_CLASSNAME(obj));
  }

  say_shader *shader = NULL;
  Data_Get_Struct(obj, say_shader, shader);

  return shader;
}

static
VALUE ray_shader_alloc(VALUE self) {
  say_shader *obj = say_shader_create();
  return Data_Wrap_Struct(self, NULL, say_shader_free, obj);
}

/*
  Make Ray's shaders forcefully use old (deprecated) style shaders.
  This should be called before the shaders get created (in the case of targets,
  this means before the creations of those targets)
*/
static
VALUE ray_shader_use_old(VALUE self) {
  say_shader_force_old();
  return Qnil;
}

/*
  @overload compile_frag(src)
    Compiles the fragment shader with a new source code.
    @param [String] src Source code
*/
static
VALUE ray_shader_compile_frag(VALUE self, VALUE src) {
  if (!say_shader_compile_frag(ray_rb2shader(self), StringValuePtr(src))) {
    rb_raise(rb_path2class("Ray::Shader::CompileError"), "%s",
             say_error_get_last());
  }
  return self;
}

/*
  @overload compile_vertex(src)
    Compiles the vertex shader with a new source code.
    @param [String] src Source code
*/
static
VALUE ray_shader_compile_vertex(VALUE self, VALUE src) {
  if (!say_shader_compile_vertex(ray_rb2shader(self), StringValuePtr(src))) {
    rb_raise(rb_path2class("Ray::Shader::CompileError"), "%s",
             say_error_get_last());
  }

  return self;
}

/*
  Links the shader.
  This is required once you have compiled the vertex and fragment shaders.
*/
static
VALUE ray_shader_link(VALUE self) {
  if (!say_shader_link(ray_rb2shader(self))) {
    rb_raise(rb_path2class("Ray::Shader::LinkError"), "%s",
             say_error_get_last());
  }

  return Qnil;
}

/*
  @overload apply_vertex(klass)
    Sets the vertex layout to use. It is required to link the vertex again for
    this change to be applied.

    @param [Class] klass Class of the vertex to use with this shader.
*/
static
VALUE ray_shader_apply_vertex(VALUE self, VALUE klass) {
  say_shader *shader = ray_rb2shader(self);

  if (klass == rb_path2class("Ray::Vertex"))
    say_shader_apply_vertex_type(shader, 0);
  else
    say_shader_apply_vertex_type(shader, ray_get_vtype(klass));

  return klass;
}

/*
  @overload locate(name)
    @param [String, Symbol] name Name of the attribute
    @return [Integer, nil] Location of the attribute
*/
static
VALUE ray_shader_locate(VALUE self, VALUE attr) {
  const char *str = rb_id2name(rb_to_id(attr));
  int loc = say_shader_locate(ray_rb2shader(self), str);

  if (loc >= 0)
    return INT2FIX(loc);
  else
    return Qnil;
}

/* Binds the shader program */
static
VALUE ray_shader_bind(VALUE self) {
  say_shader_bind(ray_rb2shader(self));
  return Qnil;
}

static
VALUE ray_shader_set_vector2(VALUE self, VALUE loc, VALUE val) {
  rb_check_frozen(self);
  rb_hash_aset(rb_iv_get(self, "@images"), loc, Qnil);
  say_shader_set_vector2_loc(ray_rb2shader(self), FIX2INT(loc),
                             ray_convert_to_vector2(val));
  return val;
}

static
VALUE ray_shader_set_vector3(VALUE self, VALUE loc, VALUE val) {
  rb_check_frozen(self);
  rb_hash_aset(rb_iv_get(self, "@images"), loc, Qnil);
  say_shader_set_vector3_loc(ray_rb2shader(self), FIX2INT(loc),
                             ray_convert_to_vector3(val));
  return val;
}

static
VALUE ray_shader_set_array(VALUE self, VALUE loc, VALUE val) {
  rb_check_frozen(self);

  rb_hash_aset(rb_iv_get(self, "@images"), loc, Qnil);

  size_t size = RARRAY_LEN(val);

  float vector[4];
  size_t id = 0;
  switch (size) {
  case 4:
    vector[id] = NUM2DBL(RAY_ARRAY_AT(val, id));
    id++;
  case 3:
    vector[id] = NUM2DBL(RAY_ARRAY_AT(val, id));
    id++;
  case 2:
    vector[id] = NUM2DBL(RAY_ARRAY_AT(val, id));
    id++;
  case 1:
    vector[id] = NUM2DBL(RAY_ARRAY_AT(val, id));
    id++;
    break;
  default:
    rb_raise(rb_eRuntimeError, "can't send %zu-sized vector",
             size);
    return Qnil;
  }

  say_shader_set_floats_loc(ray_rb2shader(self), FIX2INT(loc),
                            size, vector);
  return val;
}

static
VALUE ray_shader_set_numeric(VALUE self, VALUE loc, VALUE val) {
  rb_check_frozen(self);
  rb_hash_aset(rb_iv_get(self, "@images"), loc, Qnil);
  say_shader_set_float_loc(ray_rb2shader(self), FIX2INT(loc),
                           NUM2DBL(val));
  return val;
}

static
VALUE ray_shader_set_image(VALUE self, VALUE loc, VALUE val) {
  rb_check_frozen(self);
  rb_hash_aset(rb_iv_get(self, "@images"), loc, val);
  say_shader_set_image_loc(ray_rb2shader(self), FIX2INT(loc),
                           ray_rb2image(val));
  return val;
}

static
VALUE ray_shader_set_current_texture(VALUE self, VALUE loc) {
  rb_check_frozen(self);
  rb_hash_aset(rb_iv_get(self, "@images"), loc, Qnil);
  say_shader_set_current_texture_loc(ray_rb2shader(self), FIX2INT(loc));
  return Qnil;
}

static
VALUE ray_shader_set_bool(VALUE self, VALUE loc, VALUE val) {
  rb_check_frozen(self);
  rb_hash_aset(rb_iv_get(self, "@images"), loc, Qnil);
  say_shader_set_bool_loc(ray_rb2shader(self), FIX2INT(loc),
                          RTEST(val));
  return val;
}

static
VALUE ray_shader_set_matrix(VALUE self, VALUE loc, VALUE val) {
  rb_check_frozen(self);
  rb_hash_aset(rb_iv_get(self, "@images"), loc, Qnil);
  say_shader_set_matrix_loc(ray_rb2shader(self), FIX2INT(loc),
                          ray_rb2matrix(val));
  return val;
}

void Init_ray_shader() {
  ray_cShader = rb_define_class_under(ray_mRay, "Shader", rb_cObject);
  rb_define_class_under(ray_cShader, "CompileError", rb_eStandardError);
  rb_define_class_under(ray_cShader, "LinkError", rb_eStandardError);

  rb_define_alloc_func(ray_cShader, ray_shader_alloc);

  rb_define_singleton_method(ray_cShader, "use_old!", ray_shader_use_old, 0);

  rb_define_method(ray_cShader, "compile_frag", ray_shader_compile_frag, 1);
  rb_define_method(ray_cShader, "compile_vertex", ray_shader_compile_vertex, 1);
  rb_define_method(ray_cShader, "link", ray_shader_link, 0);

  rb_define_method(ray_cShader, "apply_vertex", ray_shader_apply_vertex, 1);

  rb_define_method(ray_cShader, "locate", ray_shader_locate, 1);

  rb_define_method(ray_cShader, "set_vector2", ray_shader_set_vector2, 2);
  rb_define_method(ray_cShader, "set_vector3", ray_shader_set_vector3, 2);
  rb_define_method(ray_cShader, "set_matrix", ray_shader_set_matrix, 2);
  rb_define_method(ray_cShader, "set_numeric", ray_shader_set_numeric, 2);
  rb_define_method(ray_cShader, "set_array", ray_shader_set_array, 2);
  rb_define_method(ray_cShader, "set_image", ray_shader_set_image, 2);
  rb_define_method(ray_cShader, "set_current_texture", ray_shader_set_current_texture, 1);
  rb_define_method(ray_cShader, "set_bool", ray_shader_set_bool, 2);

  rb_define_method(ray_cShader, "bind", ray_shader_bind, 0);
}
