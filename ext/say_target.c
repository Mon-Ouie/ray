#include "say.h"

static say_target *say_current_target = NULL;
static say_context *say_target_last_context = NULL;

static void say_target_update_states(say_target *target) {
  if (target->up_to_date) {
    target->up_to_date = 0;
    say_renderer_reset_states(target->renderer);
  }

  if (!target->view_up_to_date ||
      say_view_has_changed(target->view)) {
    say_view_apply(target->view, target->renderer->shader,
                   target->size);
    target->view_up_to_date = 1;
  }
}

say_target *say_target_create() {
  say_target *target = (say_target*)malloc(sizeof(say_target));

  target->all_contexts = say_array_create(sizeof(say_context*),
                                          say_context_free_el,
                                          NULL);

  target->context  = say_thread_variable_create();
  target->renderer = say_renderer_create();
  target->view     = say_view_create();

  target->up_to_date         = 1;
  target->view_up_to_date    = 1;
  target->own_context_needed = 1;

  target->context_proc = NULL;
  target->bind_hook    = NULL;

  return target;
}

void say_target_free(say_target *target) {
  say_view_free(target->view);
  say_renderer_free(target->renderer);

  say_thread_variable_free(target->context);

  say_array_free(target->all_contexts);

  free(target);
}

void say_target_set_context_proc(say_target *target, say_context_proc proc) {
  if (target->own_context_needed) {
    say_thread_variable_free(target->context);
    say_array_free(target->all_contexts);

    target->all_contexts = say_array_create(sizeof(say_context*),
                                            say_context_free_el,
                                            NULL);

    target->context = say_thread_variable_create();
  }

  target->context_proc = proc;
}

void say_target_need_own_contxt(say_target *target, uint8_t val) {
  target->own_context_needed = val;
}

say_context *say_target_get_context(say_target *target) {
  if (target->own_context_needed) {
    say_context *context = say_thread_variable_get(target->context);

    if (context)
      return context;
    else if (!target->context_proc) {
      say_error_set("no proc set to retrieve the context");
      return NULL;
    }
    else {
      context = target->context_proc(target->data);
      if (!context) {
        say_error_set("could not create an opengl context");
        return NULL;
      }

      say_thread_variable_set(target->context, context);

      return context;
    }
  }
  else {
    say_context_ensure();
    return say_context_current();
  }
}

void say_target_set_bind_hook(say_target *target, say_bind_hook proc) {
  target->bind_hook = proc;
}

void say_target_set_custom_data(say_target *target, void *data) {
  target->data = data;
}

int say_target_make_current(say_target *target) {
  say_context *context = say_target_get_context(target);

  if (context) {
    say_context *current = say_context_current();

    if (current == say_target_last_context &&
        target  == say_current_target &&
        current == context) {
      return 1;
    }

    target->view_up_to_date = 0;

    say_context_make_current(context);
    if (target->bind_hook)
      target->bind_hook(target->data);

    say_current_target      = target;
    say_target_last_context = context;

    return 1;
  }
  else
    return 0;
}

void say_target_set_size(say_target *target, say_vector2 size) {
  target->size = size;
  target->view_up_to_date = 0;
}

say_vector2 say_target_get_size(say_target *target) {
  return target->size;
}

void say_target_set_view(say_target *target, say_view *view) {
  say_view_copy(target->view, view);
  target->view_up_to_date = 0;
}

say_view *say_target_get_view(say_target *target) {
  return target->view;
}

say_view *say_target_get_default_view(say_target *target) {
  say_view *ret = say_view_create();
  say_view_set_size(ret, target->size);
  say_view_set_center(ret, say_make_vector2(target->size.x / 2,
                                            target->size.y / 2));
  return ret;
}

say_shader *say_target_get_shader(say_target *target) {
  return say_renderer_get_shader(target->renderer);
}

say_rect say_target_get_clip(say_target *target) {
  say_rect viewport = say_view_get_viewport(target->view);
  viewport.x *= target->size.x;
  viewport.y *= target->size.y;
  viewport.w *= target->size.x;
  viewport.h *= target->size.y;

  return viewport;
}

say_rect say_target_get_viewport_for(say_target *target, say_rect rect) {
  rect.x /= target->size.x;
  rect.y /= target->size.y;
  rect.w /= target->size.x;
  rect.h /= target->size.y;

  return rect;
}

void say_target_clear(say_target *target, say_color color) {
  if (!say_target_make_current(target))
    return;

  glClearColor(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f,
               color.a / 255.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void say_target_draw(say_target *target, say_drawable *drawable) {
  if (!say_target_make_current(target))
    return;

  if (drawable->shader) {
    say_shader_set_matrix_id(drawable->shader,
                             SAY_PROJECTION_LOC_ID,
                             say_view_get_matrix(target->view));
  }

  say_target_update_states(target);
  say_renderer_push(target->renderer, drawable);
}

void say_target_draw_buffer(say_target *target,
                            say_buffer_renderer *buf) {
  if (!say_target_make_current(target))
    return;

  buf->matrix = say_view_get_matrix(target->view);

  say_target_update_states(target);
  say_renderer_push_buffer(target->renderer, buf);
}

say_color say_target_get(say_target *target, size_t x, size_t y) {
  if (!say_target_make_current(target))
    return say_make_color(0, 0, 0, 0);

  say_pixel_bus_unbind_pack();

  say_color col;
  glReadPixels(x, (GLint)target->size.y - (GLint)y - 1, 1, 1, GL_RGBA,
               GL_UNSIGNED_BYTE, &col);

  return col;
}

say_image *say_target_get_rect(say_target *target, size_t x, size_t y,
                               size_t w, size_t h) {
  if (!say_target_make_current(target))
    return NULL;

  say_image *image = say_image_create();
  if (!say_image_create_with_size(image, w, h)) {
    say_image_free(image);
    return NULL;
  }

  say_pixel_bus_unbind_pack();
  glReadPixels(x, (GLint)target->size.y - (GLint)y - (GLint)h, w, h, GL_RGBA,
               GL_UNSIGNED_BYTE, say_image_get_buffer(image));

  return image;
}

say_image *say_target_to_image(say_target *target) {
  return say_target_get_rect(target, 0, 0, target->size.x, target->size.y);
}

void say_target_update(say_target *target) {
  say_context *context = say_target_get_context(target);
  if (context) {
    say_context_update(context);
  }

  target->up_to_date = 1;
}
