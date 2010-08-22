#include "ray.h"

VALUE ray_cJoystick = Qnil;

SDL_Joystick *ray_rb2joystick(VALUE object) {
   if (!RAY_IS_A(object, ray_cJoystick)) {
      rb_raise(rb_eTypeError, "Can't convert %s into Ray::Joystick",
               RAY_OBJ_CLASSNAME(object));
   }

   ray_joystick *joy = NULL;
   Data_Get_Struct(object, ray_joystick, joy);

   if (!joy->joystick)
      rb_raise(rb_eRuntimeError, "Trying to get a joystick that is not opened.");

   return joy->joystick;
}

void ray_free_joystick(ray_joystick *ptr) {
   if (ptr->joystick) SDL_JoystickClose(ptr->joystick);
   free(ptr);
}

VALUE ray_alloc_joystick(VALUE self) {
   ray_joystick *ptr = malloc(sizeof(ray_joystick));
   ptr->joystick = NULL;
   
   return Data_Wrap_Struct(self, NULL, ray_free_joystick, ptr);
}

/*
  Creates a new joystick, and open it. You don't need to close it explictly,
  although doing multiple call to 
  @param [Integer] stick_id Identifier for this stick. Should be smaller
                            than Ray::Joystick.count.
*/
VALUE ray_init_joystick(VALUE self, VALUE stick_id) {
   rb_iv_set(self, "@id", stick_id);
   rb_funcall2(self, RAY_METH("open"), 0, NULL);
   return Qnil;
}

/* @return [Integer] the number of joysticks available */
VALUE ray_joystick_count(VALUE self) {
   return INT2FIX(SDL_NumJoysticks());
}

/*
  @return [true, false] True if events coming from the joystick are handled.
*/
VALUE ray_joystick_handle_event(VALUE self) {
   return SDL_JoystickEventState(SDL_QUERY) ? Qtrue : Qfalse; 
}

/*
  Sets whether we should handle events coming from the joystick.
 */
VALUE ray_joystick_set_handle_event(VALUE self, VALUE val) {
   SDL_JoystickEventState(RTEST(val) ? SDL_ENABLE : SDL_DISABLE);
   return val;
}

/* Opens the joystick */
VALUE ray_joystick_open(VALUE self) {
   int id = NUM2INT(rb_iv_get(self, "@id"));
   SDL_Joystick *ptr = SDL_JoystickOpen(id);

   if (!ptr) {
      rb_raise(rb_eArgError, "Failled to open joystick %d (%s)",
               id, SDL_GetError());
   }

   ray_joystick *joy = NULL;
   Data_Get_Struct(self, ray_joystick, joy);

   joy->joystick = ptr;

   return self;
}

/* Closes the joystick. Can be called even if closed? is true. */
VALUE ray_joystick_close(VALUE self) {
   if (ray_joystick_close(self) == Qtrue)
      return Qnil;

   SDL_JoystickClose(ray_rb2joystick(self));

   ray_joystick *joy = NULL;
   Data_Get_Struct(self, ray_joystick, joy);

   joy->joystick = NULL;
   
   return Qnil;
}

/* @return [true, false] true if the joystick is closed */ 
VALUE ray_joystick_closed(VALUE self) {
   ray_joystick *joy = NULL;
   Data_Get_Struct(self, ray_joystick, joy);
   
   return joy->joystick == NULL ? Qtrue : Qfalse;
}

/* @return [Integer] The number of buttons on this joystick */
VALUE ray_joystick_button_count(VALUE self) {
   return INT2FIX(SDL_JoystickNumButtons(ray_rb2joystick(self)));
}

/* @return [Integer] The number of axes on this joystick */
VALUE ray_joystick_axe_count(VALUE self) {
   return INT2FIX(SDL_JoystickNumAxes(ray_rb2joystick(self)));
}

/* @return [Integer] The number of trackballs on this joystick */
VALUE ray_joystick_ball_count(VALUE self) {
   return INT2FIX(SDL_JoystickNumBalls(ray_rb2joystick(self)));
}

/* @return [Integer] The number of hats on this joystick */
VALUE ray_joystick_hat_count(VALUE self) {
   return INT2FIX(SDL_JoystickNumHats(ray_rb2joystick(self)));
}

void Init_ray_joystick() {
   ray_cJoystick = rb_define_class_under(ray_mRay, "Joystick", rb_cObject);
   rb_define_alloc_func(ray_cJoystick, ray_alloc_joystick);
   rb_define_method(ray_cJoystick, "initialize", ray_init_joystick, 1);

   rb_define_module_function(ray_cJoystick, "count", ray_joystick_count, 0);

   rb_define_module_function(ray_cJoystick, "handle_event=",
                             ray_joystick_set_handle_event, 1);
   rb_define_module_function(ray_cJoystick, "handle_event",
                             ray_joystick_handle_event, 0);

   rb_define_method(ray_cJoystick, "open", ray_joystick_open, 0);
   rb_define_method(ray_cJoystick, "close", ray_joystick_close, 0);
   rb_define_method(ray_cJoystick, "closed?", ray_joystick_closed, 0);
   
   rb_define_method(ray_cJoystick, "button_count", ray_joystick_button_count, 0);
   rb_define_method(ray_cJoystick, "axe_count", ray_joystick_axe_count, 0);
   rb_define_method(ray_cJoystick, "hat_count", ray_joystick_hat_count, 0);
   rb_define_method(ray_cJoystick, "ball_count", ray_joystick_ball_count, 0);
}
