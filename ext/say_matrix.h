#ifndef SAY_MATRIX_H_
#define SAY_MATRIX_H_

#include "say_basic_type.h"

typedef struct {
  float content[16];
} say_matrix;

say_matrix *say_matrix_identity();
void say_matrix_free(say_matrix *matrix);

say_matrix *say_matrix_translation(float x, float y, float z);
say_matrix *say_matrix_scale(float x, float y, float z);
say_matrix *say_matrix_rotation(float angle, float x, float y, float z);
say_matrix *say_matrix_ortho(float left, float right, float bottom, float top,
                             float z_near, float z_far);
say_matrix *say_matrix_perspective(float fovy, float aspect,
                                   float z_near, float z_far);
say_matrix *say_matrix_looking_at(float eye_x, float eye_y, float eye_z,
                                  float center_x, float center_y,
                                  float center_z,
                                  float up_x, float up_y, float up_z);

void say_matrix_set(say_matrix *matrix, int x, int y, float value);
float say_matrix_get(say_matrix *matrix, int x, int y);

say_vector3 say_matrix_transform(say_matrix *matrix, say_vector3 init);

float say_matrix_cofactor(say_matrix *matrix, int x, int y);
say_matrix *say_matrix_comatrix(say_matrix *matrix);
say_matrix *say_matrix_inverse(say_matrix *matrix);

void say_matrix_set_content(say_matrix *matrix, float *content);
float *say_matrix_get_content(say_matrix *matrix);

void say_matrix_reset(say_matrix *matrix);

void say_matrix_multiply_by(say_matrix *matrix, say_matrix *other);
void say_matrix_translate_by(say_matrix *matrix, float x, float y, float z);
void say_matrix_scale_by(say_matrix *matrix, float x, float y, float z);
void say_matrix_rotate(say_matrix *matrix, float angle, float x, float y,
                       float z);
void say_matrix_set_ortho(say_matrix *matrix,
                          float left, float right, float bottom, float top,
                          float z_near, float z_far);
void say_matrix_set_perspective(say_matrix *matrix,
                                float fovy, float aspect,
                                float z_near, float z_far);
void say_matrix_look_at(say_matrix *matrix,
                        float eye_x, float eye_y, float eye_z,
                        float center_x, float center_y, float center_z,
                        float up_x, float up_y, float up_z);

void say_matrix_set_transformation(say_matrix *matrix,
                                   say_vector2 origin,
                                   say_vector2 pos, float z,
                                   say_vector2 scale,
                                   float angle);

#endif
