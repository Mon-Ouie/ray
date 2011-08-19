#include "say.h"

static GLuint say_current_program = 0;
static say_context *say_shader_last_context = NULL;

static void say_shader_make_current(GLuint program) {
  say_context *context = say_context_current();

  if (context != say_shader_last_context ||
      program != say_current_program) {
    say_current_program     = program;
    say_shader_last_context = context;

    glUseProgram(program);
  }
}

static int say_shader_create_shader(GLuint shader, const char *src) {
  GLint length = strlen(src);
  glShaderSource(shader, 1, &src, &length);

  glCompileShader(shader);

  GLint worked = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &worked);

  if (worked != GL_TRUE) {
    GLint error_length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &error_length);

    char *error = malloc((error_length + 1) * sizeof(char));
    memset(error, '\0', (error_length + 1) * sizeof(char));

    glGetShaderInfoLog(shader, error_length, &error_length, error);
    say_error_set(error);

    free(error);
  }

  return worked;
}

static void say_shader_find_locations(say_shader *shader) {
  shader->locations[SAY_PROJECTION_LOC_ID] =
    glGetUniformLocation(shader->program, SAY_PROJECTION_ATTR);
  shader->locations[SAY_MODEL_VIEW_LOC_ID] =
    glGetUniformLocation(shader->program, SAY_MODEL_VIEW_ATTR);

  shader->locations[SAY_TEXTURE_LOC_ID] =
    glGetUniformLocation(shader->program, SAY_TEXTURE_ATTR);
  shader->locations[SAY_TEXTURE_ENABLED_LOC_ID] =
    glGetUniformLocation(shader->program, SAY_TEXTURE_ENABLED_ATTR);
}

static const char *say_default_frag_shader =
  "#version 110\n"
  "\n"
  "uniform sampler2D in_Texture;\n"
  "uniform bool in_TextureEnabled;\n"
  "\n"
  "varying vec4 var_Color;\n"
  "varying vec2 var_TexCoord;\n"
  "\n"
  "void main() {\n"
  "  if (in_TextureEnabled)\n"
  "    gl_FragColor = texture2D(in_Texture, var_TexCoord) * var_Color;\n"
  "  else\n"
  "    gl_FragColor = var_Color;\n"
  "}\n";

static const char *say_default_vertex_shader =
  "#version 110\n"
  "\n"
  "attribute vec2 in_Vertex;\n"
  "attribute vec4 in_Color;\n"
  "attribute vec2 in_TexCoord;\n"
  "\n"
  "uniform mat4 in_ModelView;\n"
  "uniform mat4 in_Projection;\n"
  "\n"
  "varying vec4 var_Color;\n"
  "varying vec2 var_TexCoord;\n"
  "\n"
  "void main() {\n"
  "  gl_Position  = vec4(in_Vertex, 0, 1) * (in_ModelView * in_Projection);\n"
  "  var_Color    = in_Color;\n"
  "  var_TexCoord = in_TexCoord;\n"
  "}\n";

static const char *say_new_default_frag_shader =
  "#version 130\n"
  "\n"
  "uniform sampler2D in_Texture;\n"
  "uniform bool in_TextureEnabled;\n"
  "\n"
  "in vec4 var_Color;\n"
  "in vec2 var_TexCoord;\n"
  "\n"
  "out vec4 out_FragColor;\n"
  "\n"
  "void main() {\n"
  "  if (in_TextureEnabled)\n"
  "    out_FragColor = texture(in_Texture, var_TexCoord) * var_Color;\n"
  "  else\n"
  "    out_FragColor = var_Color;\n"
  "}\n";

static const char *say_new_default_vertex_shader =
  "#version 130\n"
  "\n"
  "in vec2 in_Vertex;\n"
  "in vec4 in_Color;\n"
  "in vec2 in_TexCoord;\n"
  "\n"
  "uniform mat4 in_ModelView;\n"
  "uniform mat4 in_Projection;\n"
  "\n"
  "out vec4 var_Color;\n"
  "out vec2 var_TexCoord;\n"
  "\n"
  "void main() {\n"
  "  gl_Position  = vec4(in_Vertex, 0, 1) * (in_ModelView * in_Projection);\n"
  "  var_Color    = in_Color;\n"
  "  var_TexCoord = in_TexCoord;\n"
  "}\n";

static uint8_t say_shader_use_new       = 0;
static uint8_t say_shader_use_old_force = 0;

void say_shader_enable_new_glsl() {
  if (!say_shader_use_old_force)
    say_shader_use_new = 1;
}

void say_shader_force_old() {
  say_shader_use_old_force = 1;
}

bool say_shader_is_geometry_available() {
  say_context_ensure();
  return GLEW_ARB_geometry_shader4 || GLEW_VERSION_3_2;
}

say_shader *say_shader_create() {
  say_context_ensure();

  say_shader *shader = (say_shader*)malloc(sizeof(say_shader));

  shader->frag_shader     = glCreateShader(GL_FRAGMENT_SHADER);
  shader->vertex_shader   = glCreateShader(GL_VERTEX_SHADER);
  shader->geometry_shader = 0;

  bool new_shader = say_shader_use_new &&
    (!say_shader_use_old_force ||
     say_context_get_config()->core_profile);

  if (!new_shader) {
    say_shader_compile_frag(shader, say_default_frag_shader);
    say_shader_compile_vertex(shader, say_default_vertex_shader);
  }
  else {
    say_shader_compile_frag(shader, say_new_default_frag_shader);
    say_shader_compile_vertex(shader, say_new_default_vertex_shader);
  }

  shader->program = glCreateProgram();

  glAttachShader(shader->program, shader->frag_shader);
  glAttachShader(shader->program, shader->vertex_shader);

  say_shader_apply_vertex_type(shader, 0);

  if (new_shader)
    glBindFragDataLocation(shader->program, 0, "out_FragColor");

  glLinkProgram(shader->program);

  say_shader_find_locations(shader);

  say_matrix *identity = say_matrix_identity();
  say_shader_set_matrix(shader, SAY_MODEL_VIEW_ATTR, identity);
  say_shader_set_matrix(shader, SAY_PROJECTION_ATTR, identity);
  say_matrix_free(identity);

  say_shader_set_current_texture(shader, SAY_TEXTURE_ATTR);

  /* Weirldy, setting this avoid errors when setting uniforms */
  say_shader_make_current(0);

  return shader;
}

void say_shader_free(say_shader *shader) {
  say_context_ensure();

  glDeleteShader(shader->frag_shader);
  glDeleteShader(shader->vertex_shader);

  say_shader_detach_geometry(shader);

  glDeleteProgram(shader->program);

  free(shader);
}

bool say_shader_compile_frag(say_shader *shader, const char *src) {
  say_context_ensure();
  return say_shader_create_shader(shader->frag_shader, src);
}

bool say_shader_compile_vertex(say_shader *shader, const char *src) {
  say_context_ensure();
  return say_shader_create_shader(shader->vertex_shader, src);
}

bool say_shader_compile_geometry(say_shader *shader, const char *src) {
  say_context_ensure();
  if (!(GLEW_ARB_geometry_shader4 || GLEW_VERSION_3_2)) {
    say_error_set("geometry shaders aren't available");
    return false;
  }

  shader->geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
  glAttachShader(shader->program, shader->geometry_shader);
  return say_shader_create_shader(shader->geometry_shader, src);
}

void say_shader_detach_geometry(say_shader *shader) {
  if (shader->geometry_shader) {
    say_context_ensure();
    glDetachShader(shader->program, shader->geometry_shader);
    glDeleteShader(shader->geometry_shader);
    shader->geometry_shader = 0;
  }
}

void say_shader_apply_vertex_type(say_shader *shader, size_t vtype) {
  say_context_ensure();

  say_vertex_type *type = say_get_vertex_type(vtype);
  for (size_t i = 0; i < say_vertex_type_get_elem_count(type); i++) {
    glBindAttribLocation(shader->program, i + 1,
                         say_vertex_type_get_name(type, i));
  }
}

int say_shader_link(say_shader *shader) {
  say_context_ensure();
  glLinkProgram(shader->program);

  GLint worked = 0;
  glGetProgramiv(shader->program, GL_LINK_STATUS, &worked);

  if (!worked) {
    GLint error_length = 0;
    glGetProgramiv(shader->program, GL_INFO_LOG_LENGTH, &error_length);

    char *error = malloc((error_length + 1) * sizeof(char));
    memset(error, '\0', (error_length + 1) * sizeof(char));

    glGetProgramInfoLog(shader->program, error_length, &error_length, error);
    say_error_set(error);

    free(error);
  }
  else
    say_shader_find_locations(shader);

  return worked;
}

void say_shader_set_matrix(say_shader *shader, const char *name,
                           say_matrix *matrix) {
  say_shader_bind(shader);
  GLint location = glGetUniformLocation(shader->program, name);
  glUniformMatrix4fv(location, 1, GL_FALSE, matrix->content);
}

void say_shader_set_current_texture(say_shader *shader, const char *name) {
  say_shader_bind(shader);
  GLint location = glGetUniformLocation(shader->program, name);
  glUniform1i(location, 0);
}

void say_shader_set_int(say_shader *shader, const char *name, int val) {
  say_shader_bind(shader);
  GLint location = glGetUniformLocation(shader->program, name);
  glUniform1i(location, val);
}

void say_shader_set_matrix_id(say_shader *shader, say_attr_loc_id id,
                              say_matrix *matrix) {
  say_shader_bind(shader);
  glUniformMatrix4fv(shader->locations[id], 1, GL_FALSE, matrix->content);
}

void say_shader_set_current_texture_id(say_shader *shader, say_attr_loc_id id) {
  say_shader_bind(shader);
  glUniform1i(shader->locations[id], 0);
}

void say_shader_set_int_id(say_shader *shader, say_attr_loc_id id, int val) {
  say_shader_bind(shader);
  glUniform1i(shader->locations[id], val);
}

void say_shader_bind(say_shader *shader) {
  say_context_ensure();
  say_shader_make_current(shader->program);
}

int say_shader_locate(say_shader *shader, const char *name) {
  return glGetUniformLocation(shader->program, name);
}

void say_shader_set_vector2_loc(say_shader *shader, int loc, say_vector2 val) {
  say_shader_bind(shader);
  glUniform2f(loc, val.x, val.y);
}

void say_shader_set_vector3_loc(say_shader *shader, int loc, say_vector3 val) {
  say_shader_bind(shader);
  glUniform3f(loc, val.x, val.y, val.z);
}

void say_shader_set_color_loc(say_shader *shader, int loc, say_color val) {
  say_shader_bind(shader);
  float arg[4] = {val.r / 255.0, val.g / 255.0, val.b / 255.0, val.a / 255.0};
  glUniform4fv(loc, 1, arg);
}

void say_shader_set_matrix_loc(say_shader *shader, int loc, say_matrix *val) {
  say_shader_bind(shader);
  glUniformMatrix4fv(loc, 1, GL_FALSE, val->content);
}

void say_shader_set_float_loc(say_shader *shader, int loc, float val) {
  say_shader_bind(shader);
  glUniform1f(loc, val);
}

void say_shader_set_floats_loc(say_shader *shader, int loc, size_t count,
                               float *val) {
  say_shader_bind(shader);

  switch (count) {
  case 1:
    glUniform1fv(loc, 1, val);
    break;
  case 2:
    glUniform2fv(loc, 1, val);
    break;
  case 3:
    glUniform3fv(loc, 1, val);
    break;
  case 4:
    glUniform4fv(loc, 1, val);
    break;
  }
}

void say_shader_set_image_loc(say_shader *shader, int loc, say_image *val) {
  say_shader_bind(shader);
  glUniform1i(loc, val->texture);
}

void say_shader_set_current_texture_loc(say_shader *shader, int loc) {
  say_shader_bind(shader);
  glUniform1i(loc, 0);
}

void say_shader_set_bool_loc(say_shader *shader, int loc, uint8_t val) {
  say_shader_bind(shader);
  glUniform1i(loc, val);
}

GLuint say_shader_get_program(say_shader *shader) {
  return shader->program;
}
