#ifndef SAY_IMAGE_TARGET_H_
#define SAY_IMAGE_TARGET_H_

#include "say_target.h"
#include "say_image.h"

typedef struct {
  GLuint fbo, rbo;
  say_image *img;
  say_target *target;
} say_image_target;

bool say_image_target_is_available();

say_image_target *say_image_target_create();
void say_image_target_free(say_image_target *target);

void say_image_target_set_image(say_image_target *target, say_image *image);
say_image *say_image_target_get_image(say_image_target *target);
void say_image_target_update(say_image_target *taget);

void say_image_target_bind(say_image_target *target);
void say_image_target_unbind();

#endif
