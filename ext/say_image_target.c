#include "say.h"

typedef struct {
  GLuint       id;
  say_image   *img;
  say_context *ctxt;
} say_fbo;

static say_context *say_image_target_make_context(void *data) {
  return say_context_create();
}

static void say_fbo_delete_current(void *data) {
  say_fbo *fbo = (say_fbo*)data;

  if (fbo->ctxt == say_context_current() && fbo->id) {
    glDeleteFramebuffers(1, &fbo->id);
  }
}

static void say_fbo_set_zero(void *fbo) {
  *(say_fbo*)fbo = (say_fbo){0, NULL, NULL};
}

void say_fbo_make_current(GLuint fbo);
void say_rbo_make_current(GLuint rbo);

static void say_fbo_build(say_image_target *target, say_fbo *fbo) {
  if (!fbo->id)
    glGenFramebuffers(1, &fbo->id);

  say_fbo_make_current(fbo->id);

  say_image_bind(target->img);
  glGenerateMipmap(GL_TEXTURE_2D);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D, target->img->texture, 0);

  say_rbo_make_current(target->rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                        say_image_get_width(target->img),
                        say_image_get_height(target->img));

  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, target->rbo);

  fbo->img = target->img;
}

static GLuint say_current_fbo = 0;
static say_context *say_fbo_last_context = NULL;

void say_fbo_make_current(GLuint fbo) {
  say_context *context = say_context_current();

  if (context != say_fbo_last_context ||
      fbo != say_current_fbo) {
    say_current_fbo = fbo;
    say_fbo_last_context = context;

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  }
}

static GLuint say_current_rbo = 0;
static say_context *say_rbo_last_context = NULL;

void say_rbo_make_current(GLuint rbo) {
  say_context *context = say_context_current();

  if (context != say_rbo_last_context ||
      rbo != say_current_rbo) {
    say_current_rbo = rbo;
    say_rbo_last_context = context;

    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  }
}

void say_image_target_will_delete(say_array *fbos, GLuint rbo) {
  say_fbo *fbo = say_array_get(fbos, 0);
  for (; fbo; say_array_next(fbos, (void**)&fbo)) {
    if (fbo->id == say_current_fbo) {
      say_current_fbo = 0;
      break;
    }
  }

  if (say_current_rbo == rbo)
    say_current_rbo = 0;
}

bool say_image_target_is_available() {
  say_context_ensure();
  return GLEW_EXT_framebuffer_object || GLEW_VERSION_3_0;
}

say_image_target *say_image_target_create() {
  say_context_ensure();
  say_image_target *target = malloc(sizeof(say_image_target));

  target->target = say_target_create();
  target->img    = NULL;

  target->fbos = say_array_create(sizeof(say_fbo),
                                  say_fbo_delete_current,
                                  say_fbo_set_zero);

  glGenRenderbuffers(1, &target->rbo);

  return target;
}

void say_image_target_free(say_image_target *target) {
  say_context_ensure();
  say_image_target_will_delete(target->fbos, target->rbo);

  glDeleteRenderbuffers(1, &target->rbo);
  say_array_free(target->fbos);

  say_target_free(target->target);
  free(target);
}

void say_image_target_set_image(say_image_target *target, say_image *image) {
  say_context_ensure();
  target->img = image;

  if (target->img) {
    say_target_set_custom_data(target->target, target);
    say_target_set_context_proc(target->target, say_image_target_make_context);
    say_target_set_bind_hook(target->target, (say_bind_hook)say_image_target_bind);

    say_vector2 size = say_image_get_size(image);

    say_target_set_size(target->target, size);
    say_view_set_size(target->target->view, size);
    say_view_set_center(target->target->view, say_make_vector2(size.x / 2.0,
                                                               size.y / 2.0));

    say_target_make_current(target->target);
  }
}

say_image *say_image_target_get_image(say_image_target *target) {
  return target->img;
}

void say_image_target_update(say_image_target *target) {
  if (target->img) {
    say_target_update(target->target);
    say_image_mark_out_of_date(target->img);
  }
}

void say_image_target_bind(say_image_target *target) {
  say_context_ensure();

  /*
   * As FBOs aren't shared, we need to fetch the FBO for the current context. If
   * we don't find one, we need to build it.
   */
  say_context *ctxt = say_context_current();
  if (say_array_get_size(target->fbos) <= ctxt->count)
    say_array_resize(target->fbos, ctxt->count + 1);

  say_fbo *fbo = say_array_get(target->fbos, ctxt->count);
  fbo->ctxt = ctxt;

  if (fbo->img != target->img && target->img)
    say_fbo_build(target, fbo); /* also makes it current */
  else
    say_fbo_make_current(fbo->id);

  /*
   * Needed to avoid having garbage data there.
   */
  glClear(GL_DEPTH_BUFFER_BIT);
}

void say_image_target_unbind() {
  if (say_image_target_is_available())
    say_fbo_make_current(0);
}
