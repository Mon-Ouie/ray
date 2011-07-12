#include "say.h"

static void say_vector_normalize(float *x, float *y, float *z) {
  float size = sqrtf((*x) * (*x) + (*y) * (*y) + (*z) * (*z));

  *x = *x / size;
  *y = *y / size;
  *z = *z / size;
}

static say_vector3 say_vector_product(say_vector3 a, say_vector3 b) {
  say_vector3 res;

  res.x = a.y * b.z - a.z * b.y;
  res.y = a.z * b.x - a.x * b.z;
  res.z = a.x * b.y - a.y * b.x;

  return res;
}

say_matrix *say_matrix_identity() {
  say_matrix *matrix = (say_matrix*)malloc(sizeof(say_matrix));
  say_matrix_reset(matrix);
  return matrix;
}

void say_matrix_free(say_matrix *matrix) {
  free(matrix);
}

say_matrix *say_matrix_translation(float x, float y, float z) {
  say_matrix *matrix = say_matrix_identity();

  say_matrix_set(matrix, 3, 0, x);
  say_matrix_set(matrix, 3, 1, y);
  say_matrix_set(matrix, 3, 2, z);

  return matrix;
}

say_matrix *say_matrix_scale(float x, float y, float z) {
  say_matrix *matrix = say_matrix_identity();

  say_matrix_set(matrix, 0, 0, x);
  say_matrix_set(matrix, 1, 1, y);
  say_matrix_set(matrix, 2, 2, z);

  return matrix;
}

say_matrix *say_matrix_rotation(float angle, float x, float y, float z) {
  say_matrix *matrix = say_matrix_identity();

  say_vector_normalize(&x, &y, &z);

  angle = (angle * SAY_PI) / 180;

  float c = cosf(angle);
  float s = sinf(angle);

  say_matrix_set(matrix, 0, 0, x * x * (1 - c) + c);
  say_matrix_set(matrix, 1, 0, x * y * (1 - c) - z * s);
  say_matrix_set(matrix, 2, 0, x * z * (1 - c) + y * s);

  say_matrix_set(matrix, 0, 1, y * x * (1 - c) + z * s);
  say_matrix_set(matrix, 1, 1, y * y * (1 - c) + c);
  say_matrix_set(matrix, 2, 1, y * z * (1 - c) - x * s);

  say_matrix_set(matrix, 0, 2, z * x * (1 - c) - y * s);
  say_matrix_set(matrix, 1, 2, z * y * (1 - c) + x * s);
  say_matrix_set(matrix, 2, 2, z * z * (1 - c) + c);

  return matrix;
}

say_matrix *say_matrix_ortho(float left, float right, float bottom, float top,
                             float z_near, float z_far) {
  say_matrix *matrix = say_matrix_identity();

  say_matrix_set(matrix, 0, 0, +2 / (right - left));
  say_matrix_set(matrix, 1, 1, +2 / (top - bottom));
  say_matrix_set(matrix, 2, 2, -2 / (z_far - z_near));

  say_matrix_set(matrix, 3, 0, -(right + left) / (right - left));
  say_matrix_set(matrix, 3, 1, -(top + bottom) / (top - bottom));
  say_matrix_set(matrix, 3, 2, -(z_far + z_near) / (z_far - z_near));

  return matrix;
}

say_matrix *say_matrix_perspective(float fovy, float aspect,
                                   float z_near, float z_far) {
  say_matrix *matrix = say_matrix_identity();

  float f = 1.0f / tanf(fovy / 2);

  say_matrix_set(matrix, 0, 0, f / aspect);
  say_matrix_set(matrix, 1, 1, f);
  say_matrix_set(matrix, 2, 2, (z_far + z_near) / (z_near - z_far));

  say_matrix_set(matrix, 3, 2, (2 * z_far * z_near) / (z_near - z_far));
  say_matrix_set(matrix, 2, 3, -1);

  return matrix;
}

say_matrix *say_matrix_looking_at(float eye_x, float eye_y, float eye_z,
                                  float center_x, float center_y,
                                  float center_z,
                                  float up_x, float up_y, float up_z) {
  say_matrix *matrix = say_matrix_identity();

  say_vector3 f = {
    center_x - eye_x,
    center_y - eye_y,
    center_z - eye_z
  };

  say_vector3 up = {
    up_x, up_y, up_z
  };

  say_vector_normalize(&(f.x), &(f.y), &(f.z));
  say_vector_normalize(&(up.x), &(up.y), &(up.z));

  say_vector3 s = say_vector_product(f, up);
  say_vector3 u = say_vector_product(s, f);

  say_matrix_set(matrix, 1, 0, s.x);
  say_matrix_set(matrix, 2, 0, s.y);
  say_matrix_set(matrix, 3, 0, s.z);

  say_matrix_set(matrix, 1, 1, u.x);
  say_matrix_set(matrix, 2, 1, u.y);
  say_matrix_set(matrix, 3, 1, u.z);

  say_matrix_set(matrix, 1, 2, -f.x);
  say_matrix_set(matrix, 2, 2, -f.y);
  say_matrix_set(matrix, 3, 2, -f.z);

  return matrix;
}

void say_matrix_set(say_matrix *matrix, int x, int y, float value) {
  matrix->content[y * 4 + x] = value;
}

float say_matrix_get(say_matrix *matrix, int x, int y) {
  return matrix->content[y * 4 + x];
}

say_vector3 say_matrix_transform(say_matrix *matrix, say_vector3 point) {
  float *ary = matrix->content;

  say_vector3 res;

  res.x = ary[0] * point.x + ary[1] * point.y + ary[2]  * point.z + ary[3];
  res.y = ary[4] * point.x + ary[5] * point.y + ary[6]  * point.z + ary[7];
  res.z = ary[8] * point.x + ary[9] * point.y + ary[10] * point.z + ary[11];

  return res;
}

float say_matrix_cofactor(say_matrix *matrix, int fact_x, int fact_y) {
  float ary[9];
  int current_index = 0;

  for (int y = 0; y < 4; y++) {
    if (fact_y == y)
      continue;

    for (int x = 0; x < 4; x++) {
      if (fact_x == x)
        continue;

      ary[current_index++] = matrix->content[x + 4 * y];
    }
  }

  float min =
    ary[0] * ary[4] * ary[8] +
    ary[1] * ary[5] * ary[6] +
    ary[2] * ary[3] * ary[7] -

    ary[6] * ary[4] * ary[2] -
    ary[7] * ary[5] * ary[0] -
    ary[8] * ary[3] * ary[1];

  return min * ((fact_x + fact_y) % 2 == 0 ? 1 : -1);
}

say_matrix *say_matrix_comatrix(say_matrix *matrix) {
  float res[16];
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      res[x + y * 4] =  say_matrix_cofactor(matrix, x, y);
    }
  }

  say_matrix *comatrix = say_matrix_identity();
  say_matrix_set_content(comatrix, res);

  return comatrix;
}

say_matrix *say_matrix_inverse(say_matrix *matrix) {
  float *ary = matrix->content;
  say_matrix *comatrix = say_matrix_comatrix(matrix);

  float det =
    ary[0] * comatrix->content[0] +
    ary[1] * comatrix->content[1] +
    ary[2] * comatrix->content[2] +
    ary[3] * comatrix->content[3];

  float res[16];
  if (det != 0.f) {
    for (int y = 0; y < 4; y++) {
      for (int x = 0; x < 4; x++) {
        res[y + x * 4] = comatrix->content[x + y * 4] / det;
      }
    }
  }

  /* Reuse comatrix to store the output */
  say_matrix_set_content(comatrix, res);

  return comatrix;
}

void say_matrix_set_content(say_matrix *matrix, float *content) {
  memcpy(matrix->content, content, sizeof(float) * 16);
}

float *say_matrix_get_content(say_matrix *matrix) {
  return matrix->content;
}

void say_matrix_reset(say_matrix *matrix) {
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      if (x == y)
        say_matrix_set(matrix, x, y, 1.0f);
      else
        say_matrix_set(matrix, x, y, 0.0f);
    }
  }
}

void say_matrix_multiply_by(say_matrix *matrix, say_matrix *other) {
  float new_content[16];

  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      float res = 0;

      for (int i = 0; i < 4; i++)
        res += say_matrix_get(matrix, i, y) * say_matrix_get(other, x, i);

      new_content[y * 4 + x] = res;
    }
  }

  say_matrix_set_content(matrix, new_content);
}

void say_matrix_translate_by(say_matrix *matrix, float x, float y, float z) {
  say_matrix *other = say_matrix_translation(x, y, z);
  say_matrix_multiply_by(matrix, other);
  say_matrix_free(other);
}

void say_matrix_scale_by(say_matrix *matrix, float x, float y, float z) {
  say_matrix *other = say_matrix_scale(x, y, z);
  say_matrix_multiply_by(matrix, other);
  say_matrix_free(other);
}

void say_matrix_rotate(say_matrix *matrix, float angle, float x, float y,
                       float z) {
  say_matrix *other = say_matrix_rotation(angle, x, y, z);
  say_matrix_multiply_by(matrix, other);
  say_matrix_free(other);
}

void say_matrix_set_ortho(say_matrix *matrix,
                          float left, float right, float bottom, float top,
                          float z_near, float z_far) {
  say_matrix *other = say_matrix_ortho(left, right, bottom, top, z_near, z_far);
  say_matrix_multiply_by(matrix, other);
  say_matrix_free(other);
}

void say_matrix_set_perspective(say_matrix *matrix,
                                float fovy, float aspect,
                                float z_near, float z_far) {
  say_matrix *other = say_matrix_perspective(fovy, aspect, z_near, z_far);
  say_matrix_multiply_by(matrix, other);
  say_matrix_free(other);
}

void say_matrix_look_at(say_matrix *matrix,
                        float eye_x, float eye_y, float eye_z,
                        float center_x, float center_y, float center_z,
                        float up_x, float up_y, float up_z) {
  say_matrix *other = say_matrix_looking_at(eye_x, eye_y, eye_z,
                                            center_x, center_y, center_z,
                                            up_x, up_y, up_z);

  say_matrix_multiply_by(matrix, other);
  say_matrix_translate_by(matrix, -eye_x, -eye_y, -eye_z);

  say_matrix_free(other);
}
