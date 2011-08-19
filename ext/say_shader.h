#ifndef SAY_SHADER_H_
#define SAY_SHADER_H_

#include "say_basic_type.h"
#include "say_matrix.h"
#include "say_image.h"

typedef enum {
  SAY_POS_ID = 0,
  SAY_COLOR_ID,
  SAY_TEX_COORD_ID,

  SAY_ATTR_COUNT
} say_attr_id;

#define SAY_POS_ATTR             "in_Vertex"
#define SAY_COLOR_ATTR           "in_Color"
#define SAY_TEX_COORD_ATTR       "in_TexCoord"

#define SAY_PROJECTION_ATTR      "in_Projection"
#define SAY_MODEL_VIEW_ATTR      "in_ModelView"

#define SAY_TEXTURE_ATTR         "in_Texture"
#define SAY_TEXTURE_ENABLED_ATTR "in_TextureEnabled"

#define SAY_FRAG_COLOR           "out_FragColor"

typedef enum {
  SAY_PROJECTION_LOC_ID = 0,
  SAY_MODEL_VIEW_LOC_ID,

  SAY_TEXTURE_LOC_ID,
  SAY_TEXTURE_ENABLED_LOC_ID,

  SAY_LOC_ID_COUNT
} say_attr_loc_id;

typedef struct {
  GLuint program;

  GLuint frag_shader;
  GLuint vertex_shader;
  GLuint geometry_shader;

  GLint locations[SAY_LOC_ID_COUNT];
} say_shader;

bool say_shader_is_geometry_available();

say_shader *say_shader_create();
void say_shader_free(say_shader *shader);

void say_shader_enable_new_glsl();
void say_shader_force_old();

bool say_shader_compile_frag(say_shader *shader, const char *src);
bool say_shader_compile_vertex(say_shader *shader, const char *src);
bool say_shader_compile_geometry(say_shader *shader, const char *src);

void say_shader_detach_geometry(say_shader *shader);

void say_shader_apply_vertex_type(say_shader *shader, size_t vtype);

int say_shader_link(say_shader *shader);

void say_shader_set_matrix(say_shader *shader, const char *name,
                           say_matrix *matrix);
void say_shader_set_current_texture(say_shader *shader, const char *name);
void say_shader_set_int(say_shader *shader, const char *name, int val);

void say_shader_set_matrix_id(say_shader *shader, say_attr_loc_id id,
                              say_matrix *matrix);
void say_shader_set_current_texture_id(say_shader *shader, say_attr_loc_id id);
void say_shader_set_int_id(say_shader *shader, say_attr_loc_id id, int val);

int say_shader_locate(say_shader *shader, const char *name);

void say_shader_set_vector2_loc(say_shader *shader, int loc, say_vector2 val);
void say_shader_set_vector3_loc(say_shader *shader, int loc, say_vector3 val);
void say_shader_set_color_loc(say_shader *shader, int loc, say_color val);
void say_shader_set_matrix_loc(say_shader *shader, int loc, say_matrix *val);
void say_shader_set_float_loc(say_shader *shader, int loc, float val);
void say_shader_set_floats_loc(say_shader *shader, int loc, size_t count,
                               float *val);
void say_shader_set_image_loc(say_shader *shader, int loc, say_image *val);
void say_shader_set_current_texture_loc(say_shader *shader, int loc);
void say_shader_set_bool_loc(say_shader *shader, int loc, uint8_t val);

void say_shader_bind(say_shader *shader);

GLuint say_shader_get_program(say_shader *shader);

#endif
