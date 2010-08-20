#include "ray.h"

VALUE ray_cEvent = Qnil;

SDL_Event *ray_rb2event(VALUE obj) {
   if (!RAY_IS_A(obj, ray_cEvent)) {
      rb_raise(rb_eTypeError, "Can't convert %s into Ray::Event",
               RAY_OBJ_CLASSNAME(obj));
   }

   SDL_Event *ptr;
   Data_Get_Struct(obj, SDL_Event, ptr);

   return ptr;
}

void ray_free_event(SDL_Event *ptr) {
   free(ptr);
}

VALUE ray_alloc_event(VALUE self) {
   SDL_Event *ptr = malloc(sizeof(SDL_Event));
   VALUE ret = Data_Wrap_Struct(self, 0L, ray_free_event, ptr);

   return ret;
}

/*
  Creates the event according to what's happening now.
*/
VALUE ray_init_event(VALUE self) {
   SDL_Event *event = ray_rb2event(self);
   SDL_PollEvent(event);

   return Qnil;
}

/*
  Waits for an event and returns it.
  @return [Ray::Event] The event that has just happened.
*/
VALUE ray_wait_event(VALUE self) {
   SDL_Event *event = malloc(sizeof(SDL_Event));
   SDL_WaitEvent(event);

   return Data_Wrap_Struct(self, 0, ray_free_event, event);
}

/*
  Updates the object according to what's happening now.
*/
VALUE ray_event_poll(VALUE self) {
   SDL_PollEvent(ray_rb2event(self));
   return self;
}

/*
  Waits for an event and updates this object.
*/
VALUE ray_event_wait(VALUE self) {
   SDL_WaitEvent(ray_rb2event(self));
   return self;
}

/*
  Returns the type of the event. See the TYPE_* constants.
*/
VALUE ray_event_type(VALUE self) {
   return INT2FIX(ray_rb2event(self)->type);
}

/*
  @return [true, false, nil] true if the key is pressed.
*/
VALUE ray_event_key_pressed(VALUE self) {
   SDL_Event *ev = ray_rb2event(self);
   if (ev->type != SDL_KEYUP && ev->type != SDL_KEYDOWN)
      return Qnil;

   return ev->key.state == SDL_PRESSED ? Qtrue : Qfalse;
}

/*
  @return [Integer, nil] An integer representing the key, if any.
*/
VALUE ray_event_key(VALUE self) {
   SDL_Event *ev = ray_rb2event(self);
   if (ev->type != SDL_KEYUP && ev->type != SDL_KEYDOWN)
      return Qnil;

   return INT2FIX(ev->key.keysym.sym);
}

/*
  @return [Integer, nil] OR'd combination of the mod keys
                         (control, alt, shift, ...) pressed
                         at the same time.
*/
VALUE ray_event_mod_keys(VALUE self) {
   SDL_Event *ev = ray_rb2event(self);
   if (ev->type != SDL_KEYUP && ev->type != SDL_KEYDOWN)
      return Qnil;

   return INT2FIX(ev->key.keysym.mod);
}

/*
  @return [Integer, nil] A mask of the focus states, for active event. 
*/
VALUE ray_event_focus_state(VALUE self) {
      SDL_Event *ev = ray_rb2event(self);
   if (ev->type != SDL_ACTIVEEVENT)
      return Qnil;
   return INT2FIX(ev->active.state);
}

/*
  @return [true, false, nil] True if this event means we gained
                             a state.
 */
VALUE ray_event_is_gain(VALUE self) {
   SDL_Event *ev = ray_rb2event(self);
   if (ev->type != SDL_ACTIVEEVENT)
      return Qnil;

   return ev->active.gain ? Qtrue : Qfalse;
}

void Init_ray_event() {
   ray_cEvent = rb_define_class_under(ray_mRay, "Event", rb_cObject);
   rb_define_alloc_func(ray_cEvent, ray_alloc_event);
   rb_define_method(ray_cEvent, "initialize", ray_init_event, 0);

   rb_define_module_function(ray_cEvent, "wait", ray_wait_event, 0);

   rb_define_method(ray_cEvent, "poll!", ray_event_poll, 0);
   rb_define_method(ray_cEvent, "wait!", ray_event_wait, 0);

   rb_define_method(ray_cEvent, "type", ray_event_type, 0);

   rb_define_method(ray_cEvent, "key_pressed?", ray_event_key_pressed, 0);
   rb_define_method(ray_cEvent, "key", ray_event_key, 0);
   rb_define_method(ray_cEvent, "mod_keys", ray_event_mod_keys, 0);
   
   rb_define_method(ray_cEvent, "focus_state", ray_event_focus_state, 0);
   rb_define_method(ray_cEvent, "is_gain?", ray_event_is_gain, 0);

   rb_define_const(ray_cEvent, "TYPE_NOEVENT", INT2FIX(SDL_NOEVENT));
   rb_define_const(ray_cEvent, "TYPE_ACTIVEEVENT", INT2FIX(SDL_ACTIVEEVENT));
   rb_define_const(ray_cEvent, "TYPE_KEYDOWN", INT2FIX(SDL_KEYDOWN));
   rb_define_const(ray_cEvent, "TYPE_KEYUP", INT2FIX(SDL_KEYUP));
   rb_define_const(ray_cEvent, "TYPE_MOUSEMOTION", INT2FIX(SDL_MOUSEMOTION));
   rb_define_const(ray_cEvent, "TYPE_MOUSEBUTTONDOWN",
                   INT2FIX(SDL_MOUSEBUTTONDOWN));
   rb_define_const(ray_cEvent, "TYPE_MOUSEBUTTONUP",
                   INT2FIX(SDL_MOUSEBUTTONUP));
   rb_define_const(ray_cEvent, "TYPE_JOYAXISMOTION",
                   INT2FIX(SDL_JOYAXISMOTION));
   rb_define_const(ray_cEvent, "TYPE_JOYBALLMOTION",
                   INT2FIX(SDL_JOYBALLMOTION));
   rb_define_const(ray_cEvent, "TYPE_JOYHATMOTION",
                   INT2FIX(SDL_JOYHATMOTION));
   rb_define_const(ray_cEvent, "TYPE_JOYBUTTONDOWN",
                   INT2FIX(SDL_JOYBUTTONDOWN));
   rb_define_const(ray_cEvent, "TYPE_JOYBUTTONUP", INT2FIX(SDL_JOYBUTTONUP));
   rb_define_const(ray_cEvent, "TYPE_QUIT", INT2FIX(SDL_QUIT));
   rb_define_const(ray_cEvent, "TYPE_SYSWMEVENT", INT2FIX(SDL_SYSWMEVENT));
   rb_define_const(ray_cEvent, "TYPE_VIDEORESIZE", INT2FIX(SDL_VIDEORESIZE));
   rb_define_const(ray_cEvent, "TYPE_VIDEOEXPOSE", INT2FIX(SDL_VIDEOEXPOSE));

   rb_define_const(ray_cEvent, "KEY_UNKNOWN", INT2FIX(SDLK_UNKNOWN));
   rb_define_const(ray_cEvent, "KEY_FIRST", INT2FIX(SDLK_FIRST));
   rb_define_const(ray_cEvent, "KEY_BACKSPACE", INT2FIX(SDLK_BACKSPACE));
   rb_define_const(ray_cEvent, "KEY_TAB", INT2FIX(SDLK_TAB));
   rb_define_const(ray_cEvent, "KEY_CLEAR", INT2FIX(SDLK_CLEAR));
   rb_define_const(ray_cEvent, "KEY_RETURN", INT2FIX(SDLK_RETURN));
   rb_define_const(ray_cEvent, "KEY_PAUSE", INT2FIX(SDLK_PAUSE));
   rb_define_const(ray_cEvent, "KEY_ESCAPE", INT2FIX(SDLK_ESCAPE));
   rb_define_const(ray_cEvent, "KEY_SPACE", INT2FIX(SDLK_SPACE));
   rb_define_const(ray_cEvent, "KEY_EXCLAIM", INT2FIX(SDLK_EXCLAIM));
   rb_define_const(ray_cEvent, "KEY_QUOTEDBL", INT2FIX(SDLK_QUOTEDBL));
   rb_define_const(ray_cEvent, "KEY_HASH", INT2FIX(SDLK_HASH));
   rb_define_const(ray_cEvent, "KEY_DOLLAR", INT2FIX(SDLK_DOLLAR));
   rb_define_const(ray_cEvent, "KEY_AMPERSAND", INT2FIX(SDLK_AMPERSAND));
   rb_define_const(ray_cEvent, "KEY_QUOTE", INT2FIX(SDLK_QUOTE));
   rb_define_const(ray_cEvent, "KEY_LEFTPAREN", INT2FIX(SDLK_LEFTPAREN));
   rb_define_const(ray_cEvent, "KEY_RIGHTPAREN", INT2FIX(SDLK_RIGHTPAREN));
   rb_define_const(ray_cEvent, "KEY_ASTERISK", INT2FIX(SDLK_ASTERISK));
   rb_define_const(ray_cEvent, "KEY_PLUS", INT2FIX(SDLK_PLUS));
   rb_define_const(ray_cEvent, "KEY_COMMA", INT2FIX(SDLK_COMMA));
   rb_define_const(ray_cEvent, "KEY_MINUS", INT2FIX(SDLK_MINUS));
   rb_define_const(ray_cEvent, "KEY_PERIOD", INT2FIX(SDLK_PERIOD));
   rb_define_const(ray_cEvent, "KEY_SLASH", INT2FIX(SDLK_SLASH));
   rb_define_const(ray_cEvent, "KEY_0", INT2FIX(SDLK_0));
   rb_define_const(ray_cEvent, "KEY_1", INT2FIX(SDLK_1));
   rb_define_const(ray_cEvent, "KEY_2", INT2FIX(SDLK_2));
   rb_define_const(ray_cEvent, "KEY_3", INT2FIX(SDLK_3));
   rb_define_const(ray_cEvent, "KEY_4", INT2FIX(SDLK_4));
   rb_define_const(ray_cEvent, "KEY_5", INT2FIX(SDLK_5));
   rb_define_const(ray_cEvent, "KEY_6", INT2FIX(SDLK_6));
   rb_define_const(ray_cEvent, "KEY_7", INT2FIX(SDLK_7));
   rb_define_const(ray_cEvent, "KEY_8", INT2FIX(SDLK_8));
   rb_define_const(ray_cEvent, "KEY_9", INT2FIX(SDLK_9));
   rb_define_const(ray_cEvent, "KEY_COLON", INT2FIX(SDLK_COLON));
   rb_define_const(ray_cEvent, "KEY_SEMICOLON", INT2FIX(SDLK_SEMICOLON));
   rb_define_const(ray_cEvent, "KEY_LESS", INT2FIX(SDLK_LESS));
   rb_define_const(ray_cEvent, "KEY_EQUALS", INT2FIX(SDLK_EQUALS));
   rb_define_const(ray_cEvent, "KEY_GREATER", INT2FIX(SDLK_GREATER));
   rb_define_const(ray_cEvent, "KEY_QUESTION", INT2FIX(SDLK_QUESTION));
   rb_define_const(ray_cEvent, "KEY_AT", INT2FIX(SDLK_AT));
   rb_define_const(ray_cEvent, "KEY_LEFTBRACKET", INT2FIX(SDLK_LEFTBRACKET));
   rb_define_const(ray_cEvent, "KEY_BACKSLASH", INT2FIX(SDLK_BACKSLASH));
   rb_define_const(ray_cEvent, "KEY_RIGHTBRACKET", INT2FIX(SDLK_RIGHTBRACKET));
   rb_define_const(ray_cEvent, "KEY_CARET", INT2FIX(SDLK_CARET));
   rb_define_const(ray_cEvent, "KEY_UNDERSCORE", INT2FIX(SDLK_UNDERSCORE));
   rb_define_const(ray_cEvent, "KEY_BACKQUOTE", INT2FIX(SDLK_BACKQUOTE));
   rb_define_const(ray_cEvent, "KEY_a", INT2FIX(SDLK_a));
   rb_define_const(ray_cEvent, "KEY_b", INT2FIX(SDLK_b));
   rb_define_const(ray_cEvent, "KEY_c", INT2FIX(SDLK_c));
   rb_define_const(ray_cEvent, "KEY_d", INT2FIX(SDLK_d));
   rb_define_const(ray_cEvent, "KEY_e", INT2FIX(SDLK_e));
   rb_define_const(ray_cEvent, "KEY_f", INT2FIX(SDLK_f));
   rb_define_const(ray_cEvent, "KEY_g", INT2FIX(SDLK_g));
   rb_define_const(ray_cEvent, "KEY_h", INT2FIX(SDLK_h));
   rb_define_const(ray_cEvent, "KEY_i", INT2FIX(SDLK_i));
   rb_define_const(ray_cEvent, "KEY_j", INT2FIX(SDLK_j));
   rb_define_const(ray_cEvent, "KEY_k", INT2FIX(SDLK_k));
   rb_define_const(ray_cEvent, "KEY_l", INT2FIX(SDLK_l));
   rb_define_const(ray_cEvent, "KEY_m", INT2FIX(SDLK_m));
   rb_define_const(ray_cEvent, "KEY_n", INT2FIX(SDLK_n));
   rb_define_const(ray_cEvent, "KEY_o", INT2FIX(SDLK_o));
   rb_define_const(ray_cEvent, "KEY_p", INT2FIX(SDLK_p));
   rb_define_const(ray_cEvent, "KEY_q", INT2FIX(SDLK_q));
   rb_define_const(ray_cEvent, "KEY_r", INT2FIX(SDLK_r));
   rb_define_const(ray_cEvent, "KEY_s", INT2FIX(SDLK_s));
   rb_define_const(ray_cEvent, "KEY_t", INT2FIX(SDLK_t));
   rb_define_const(ray_cEvent, "KEY_u", INT2FIX(SDLK_u));
   rb_define_const(ray_cEvent, "KEY_v", INT2FIX(SDLK_v));
   rb_define_const(ray_cEvent, "KEY_w", INT2FIX(SDLK_w));
   rb_define_const(ray_cEvent, "KEY_x", INT2FIX(SDLK_x));
   rb_define_const(ray_cEvent, "KEY_y", INT2FIX(SDLK_y));
   rb_define_const(ray_cEvent, "KEY_z", INT2FIX(SDLK_z));
   rb_define_const(ray_cEvent, "KEY_DELETE", INT2FIX(SDLK_DELETE));
   rb_define_const(ray_cEvent, "KEY_WORLD_0", INT2FIX(SDLK_WORLD_0));
   rb_define_const(ray_cEvent, "KEY_WORLD_1", INT2FIX(SDLK_WORLD_1));
   rb_define_const(ray_cEvent, "KEY_WORLD_2", INT2FIX(SDLK_WORLD_2));
   rb_define_const(ray_cEvent, "KEY_WORLD_3", INT2FIX(SDLK_WORLD_3));
   rb_define_const(ray_cEvent, "KEY_WORLD_4", INT2FIX(SDLK_WORLD_4));
   rb_define_const(ray_cEvent, "KEY_WORLD_5", INT2FIX(SDLK_WORLD_5));
   rb_define_const(ray_cEvent, "KEY_WORLD_6", INT2FIX(SDLK_WORLD_6));
   rb_define_const(ray_cEvent, "KEY_WORLD_7", INT2FIX(SDLK_WORLD_7));
   rb_define_const(ray_cEvent, "KEY_WORLD_8", INT2FIX(SDLK_WORLD_8));
   rb_define_const(ray_cEvent, "KEY_WORLD_9", INT2FIX(SDLK_WORLD_9));
   rb_define_const(ray_cEvent, "KEY_WORLD_10", INT2FIX(SDLK_WORLD_10));
   rb_define_const(ray_cEvent, "KEY_WORLD_11", INT2FIX(SDLK_WORLD_11));
   rb_define_const(ray_cEvent, "KEY_WORLD_12", INT2FIX(SDLK_WORLD_12));
   rb_define_const(ray_cEvent, "KEY_WORLD_13", INT2FIX(SDLK_WORLD_13));
   rb_define_const(ray_cEvent, "KEY_WORLD_14", INT2FIX(SDLK_WORLD_14));
   rb_define_const(ray_cEvent, "KEY_WORLD_15", INT2FIX(SDLK_WORLD_15));
   rb_define_const(ray_cEvent, "KEY_WORLD_16", INT2FIX(SDLK_WORLD_16));
   rb_define_const(ray_cEvent, "KEY_WORLD_17", INT2FIX(SDLK_WORLD_17));
   rb_define_const(ray_cEvent, "KEY_WORLD_18", INT2FIX(SDLK_WORLD_18));
   rb_define_const(ray_cEvent, "KEY_WORLD_19", INT2FIX(SDLK_WORLD_19));
   rb_define_const(ray_cEvent, "KEY_WORLD_20", INT2FIX(SDLK_WORLD_20));
   rb_define_const(ray_cEvent, "KEY_WORLD_21", INT2FIX(SDLK_WORLD_21));
   rb_define_const(ray_cEvent, "KEY_WORLD_22", INT2FIX(SDLK_WORLD_22));
   rb_define_const(ray_cEvent, "KEY_WORLD_23", INT2FIX(SDLK_WORLD_23));
   rb_define_const(ray_cEvent, "KEY_WORLD_24", INT2FIX(SDLK_WORLD_24));
   rb_define_const(ray_cEvent, "KEY_WORLD_25", INT2FIX(SDLK_WORLD_25));
   rb_define_const(ray_cEvent, "KEY_WORLD_26", INT2FIX(SDLK_WORLD_26));
   rb_define_const(ray_cEvent, "KEY_WORLD_27", INT2FIX(SDLK_WORLD_27));
   rb_define_const(ray_cEvent, "KEY_WORLD_28", INT2FIX(SDLK_WORLD_28));
   rb_define_const(ray_cEvent, "KEY_WORLD_29", INT2FIX(SDLK_WORLD_29));
   rb_define_const(ray_cEvent, "KEY_WORLD_30", INT2FIX(SDLK_WORLD_30));
   rb_define_const(ray_cEvent, "KEY_WORLD_31", INT2FIX(SDLK_WORLD_31));
   rb_define_const(ray_cEvent, "KEY_WORLD_32", INT2FIX(SDLK_WORLD_32));
   rb_define_const(ray_cEvent, "KEY_WORLD_33", INT2FIX(SDLK_WORLD_33));
   rb_define_const(ray_cEvent, "KEY_WORLD_34", INT2FIX(SDLK_WORLD_34));
   rb_define_const(ray_cEvent, "KEY_WORLD_35", INT2FIX(SDLK_WORLD_35));
   rb_define_const(ray_cEvent, "KEY_WORLD_36", INT2FIX(SDLK_WORLD_36));
   rb_define_const(ray_cEvent, "KEY_WORLD_37", INT2FIX(SDLK_WORLD_37));
   rb_define_const(ray_cEvent, "KEY_WORLD_38", INT2FIX(SDLK_WORLD_38));
   rb_define_const(ray_cEvent, "KEY_WORLD_39", INT2FIX(SDLK_WORLD_39));
   rb_define_const(ray_cEvent, "KEY_WORLD_40", INT2FIX(SDLK_WORLD_40));
   rb_define_const(ray_cEvent, "KEY_WORLD_41", INT2FIX(SDLK_WORLD_41));
   rb_define_const(ray_cEvent, "KEY_WORLD_42", INT2FIX(SDLK_WORLD_42));
   rb_define_const(ray_cEvent, "KEY_WORLD_43", INT2FIX(SDLK_WORLD_43));
   rb_define_const(ray_cEvent, "KEY_WORLD_44", INT2FIX(SDLK_WORLD_44));
   rb_define_const(ray_cEvent, "KEY_WORLD_45", INT2FIX(SDLK_WORLD_45));
   rb_define_const(ray_cEvent, "KEY_WORLD_46", INT2FIX(SDLK_WORLD_46));
   rb_define_const(ray_cEvent, "KEY_WORLD_47", INT2FIX(SDLK_WORLD_47));
   rb_define_const(ray_cEvent, "KEY_WORLD_48", INT2FIX(SDLK_WORLD_48));
   rb_define_const(ray_cEvent, "KEY_WORLD_49", INT2FIX(SDLK_WORLD_49));
   rb_define_const(ray_cEvent, "KEY_WORLD_50", INT2FIX(SDLK_WORLD_50));
   rb_define_const(ray_cEvent, "KEY_WORLD_51", INT2FIX(SDLK_WORLD_51));
   rb_define_const(ray_cEvent, "KEY_WORLD_52", INT2FIX(SDLK_WORLD_52));
   rb_define_const(ray_cEvent, "KEY_WORLD_53", INT2FIX(SDLK_WORLD_53));
   rb_define_const(ray_cEvent, "KEY_WORLD_54", INT2FIX(SDLK_WORLD_54));
   rb_define_const(ray_cEvent, "KEY_WORLD_55", INT2FIX(SDLK_WORLD_55));
   rb_define_const(ray_cEvent, "KEY_WORLD_56", INT2FIX(SDLK_WORLD_56));
   rb_define_const(ray_cEvent, "KEY_WORLD_57", INT2FIX(SDLK_WORLD_57));
   rb_define_const(ray_cEvent, "KEY_WORLD_58", INT2FIX(SDLK_WORLD_58));
   rb_define_const(ray_cEvent, "KEY_WORLD_59", INT2FIX(SDLK_WORLD_59));
   rb_define_const(ray_cEvent, "KEY_WORLD_60", INT2FIX(SDLK_WORLD_60));
   rb_define_const(ray_cEvent, "KEY_WORLD_61", INT2FIX(SDLK_WORLD_61));
   rb_define_const(ray_cEvent, "KEY_WORLD_62", INT2FIX(SDLK_WORLD_62));
   rb_define_const(ray_cEvent, "KEY_WORLD_63", INT2FIX(SDLK_WORLD_63));
   rb_define_const(ray_cEvent, "KEY_WORLD_64", INT2FIX(SDLK_WORLD_64));
   rb_define_const(ray_cEvent, "KEY_WORLD_65", INT2FIX(SDLK_WORLD_65));
   rb_define_const(ray_cEvent, "KEY_WORLD_66", INT2FIX(SDLK_WORLD_66));
   rb_define_const(ray_cEvent, "KEY_WORLD_67", INT2FIX(SDLK_WORLD_67));
   rb_define_const(ray_cEvent, "KEY_WORLD_68", INT2FIX(SDLK_WORLD_68));
   rb_define_const(ray_cEvent, "KEY_WORLD_69", INT2FIX(SDLK_WORLD_69));
   rb_define_const(ray_cEvent, "KEY_WORLD_70", INT2FIX(SDLK_WORLD_70));
   rb_define_const(ray_cEvent, "KEY_WORLD_71", INT2FIX(SDLK_WORLD_71));
   rb_define_const(ray_cEvent, "KEY_WORLD_72", INT2FIX(SDLK_WORLD_72));
   rb_define_const(ray_cEvent, "KEY_WORLD_73", INT2FIX(SDLK_WORLD_73));
   rb_define_const(ray_cEvent, "KEY_WORLD_74", INT2FIX(SDLK_WORLD_74));
   rb_define_const(ray_cEvent, "KEY_WORLD_75", INT2FIX(SDLK_WORLD_75));
   rb_define_const(ray_cEvent, "KEY_WORLD_76", INT2FIX(SDLK_WORLD_76));
   rb_define_const(ray_cEvent, "KEY_WORLD_77", INT2FIX(SDLK_WORLD_77));
   rb_define_const(ray_cEvent, "KEY_WORLD_78", INT2FIX(SDLK_WORLD_78));
   rb_define_const(ray_cEvent, "KEY_WORLD_79", INT2FIX(SDLK_WORLD_79));
   rb_define_const(ray_cEvent, "KEY_WORLD_80", INT2FIX(SDLK_WORLD_80));
   rb_define_const(ray_cEvent, "KEY_WORLD_81", INT2FIX(SDLK_WORLD_81));
   rb_define_const(ray_cEvent, "KEY_WORLD_82", INT2FIX(SDLK_WORLD_82));
   rb_define_const(ray_cEvent, "KEY_WORLD_83", INT2FIX(SDLK_WORLD_83));
   rb_define_const(ray_cEvent, "KEY_WORLD_84", INT2FIX(SDLK_WORLD_84));
   rb_define_const(ray_cEvent, "KEY_WORLD_85", INT2FIX(SDLK_WORLD_85));
   rb_define_const(ray_cEvent, "KEY_WORLD_86", INT2FIX(SDLK_WORLD_86));
   rb_define_const(ray_cEvent, "KEY_WORLD_87", INT2FIX(SDLK_WORLD_87));
   rb_define_const(ray_cEvent, "KEY_WORLD_88", INT2FIX(SDLK_WORLD_88));
   rb_define_const(ray_cEvent, "KEY_WORLD_89", INT2FIX(SDLK_WORLD_89));
   rb_define_const(ray_cEvent, "KEY_WORLD_90", INT2FIX(SDLK_WORLD_90));
   rb_define_const(ray_cEvent, "KEY_WORLD_91", INT2FIX(SDLK_WORLD_91));
   rb_define_const(ray_cEvent, "KEY_WORLD_92", INT2FIX(SDLK_WORLD_92));
   rb_define_const(ray_cEvent, "KEY_WORLD_93", INT2FIX(SDLK_WORLD_93));
   rb_define_const(ray_cEvent, "KEY_WORLD_94", INT2FIX(SDLK_WORLD_94));
   rb_define_const(ray_cEvent, "KEY_WORLD_95", INT2FIX(SDLK_WORLD_95));
   rb_define_const(ray_cEvent, "KEY_KP0", INT2FIX(SDLK_KP0));
   rb_define_const(ray_cEvent, "KEY_KP1", INT2FIX(SDLK_KP1));
   rb_define_const(ray_cEvent, "KEY_KP2", INT2FIX(SDLK_KP2));
   rb_define_const(ray_cEvent, "KEY_KP3", INT2FIX(SDLK_KP3));
   rb_define_const(ray_cEvent, "KEY_KP4", INT2FIX(SDLK_KP4));
   rb_define_const(ray_cEvent, "KEY_KP5", INT2FIX(SDLK_KP5));
   rb_define_const(ray_cEvent, "KEY_KP6", INT2FIX(SDLK_KP6));
   rb_define_const(ray_cEvent, "KEY_KP7", INT2FIX(SDLK_KP7));
   rb_define_const(ray_cEvent, "KEY_KP8", INT2FIX(SDLK_KP8));
   rb_define_const(ray_cEvent, "KEY_KP9", INT2FIX(SDLK_KP9));
   rb_define_const(ray_cEvent, "KEY_KP_PERIOD", INT2FIX(SDLK_KP_PERIOD));
   rb_define_const(ray_cEvent, "KEY_KP_DIVIDE", INT2FIX(SDLK_KP_DIVIDE));
   rb_define_const(ray_cEvent, "KEY_KP_MULTIPLY", INT2FIX(SDLK_KP_MULTIPLY));
   rb_define_const(ray_cEvent, "KEY_KP_MINUS", INT2FIX(SDLK_KP_MINUS));
   rb_define_const(ray_cEvent, "KEY_KP_PLUS", INT2FIX(SDLK_KP_PLUS));
   rb_define_const(ray_cEvent, "KEY_KP_ENTER", INT2FIX(SDLK_KP_ENTER));
   rb_define_const(ray_cEvent, "KEY_KP_EQUALS", INT2FIX(SDLK_KP_EQUALS));
   rb_define_const(ray_cEvent, "KEY_UP", INT2FIX(SDLK_UP));
   rb_define_const(ray_cEvent, "KEY_DOWN", INT2FIX(SDLK_DOWN));
   rb_define_const(ray_cEvent, "KEY_RIGHT", INT2FIX(SDLK_RIGHT));
   rb_define_const(ray_cEvent, "KEY_LEFT", INT2FIX(SDLK_LEFT));
   rb_define_const(ray_cEvent, "KEY_INSERT", INT2FIX(SDLK_INSERT));
   rb_define_const(ray_cEvent, "KEY_HOME", INT2FIX(SDLK_HOME));
   rb_define_const(ray_cEvent, "KEY_END", INT2FIX(SDLK_END));
   rb_define_const(ray_cEvent, "KEY_PAGEUP", INT2FIX(SDLK_PAGEUP));
   rb_define_const(ray_cEvent, "KEY_PAGEDOWN", INT2FIX(SDLK_PAGEDOWN));
   rb_define_const(ray_cEvent, "KEY_F1", INT2FIX(SDLK_F1));
   rb_define_const(ray_cEvent, "KEY_F2", INT2FIX(SDLK_F2));
   rb_define_const(ray_cEvent, "KEY_F3", INT2FIX(SDLK_F3));
   rb_define_const(ray_cEvent, "KEY_F4", INT2FIX(SDLK_F4));
   rb_define_const(ray_cEvent, "KEY_F5", INT2FIX(SDLK_F5));
   rb_define_const(ray_cEvent, "KEY_F6", INT2FIX(SDLK_F6));
   rb_define_const(ray_cEvent, "KEY_F7", INT2FIX(SDLK_F7));
   rb_define_const(ray_cEvent, "KEY_F8", INT2FIX(SDLK_F8));
   rb_define_const(ray_cEvent, "KEY_F9", INT2FIX(SDLK_F9));
   rb_define_const(ray_cEvent, "KEY_F10", INT2FIX(SDLK_F10));
   rb_define_const(ray_cEvent, "KEY_F11", INT2FIX(SDLK_F11));
   rb_define_const(ray_cEvent, "KEY_F12", INT2FIX(SDLK_F12));
   rb_define_const(ray_cEvent, "KEY_F13", INT2FIX(SDLK_F13));
   rb_define_const(ray_cEvent, "KEY_F14", INT2FIX(SDLK_F14));
   rb_define_const(ray_cEvent, "KEY_F15", INT2FIX(SDLK_F15));
   rb_define_const(ray_cEvent, "KEY_NUMLOCK", INT2FIX(SDLK_NUMLOCK));
   rb_define_const(ray_cEvent, "KEY_CAPSLOCK", INT2FIX(SDLK_CAPSLOCK));
   rb_define_const(ray_cEvent, "KEY_SCROLLOCK", INT2FIX(SDLK_SCROLLOCK));
   rb_define_const(ray_cEvent, "KEY_RSHIFT", INT2FIX(SDLK_RSHIFT));
   rb_define_const(ray_cEvent, "KEY_LSHIFT", INT2FIX(SDLK_LSHIFT));
   rb_define_const(ray_cEvent, "KEY_RCTRL", INT2FIX(SDLK_RCTRL));
   rb_define_const(ray_cEvent, "KEY_LCTRL", INT2FIX(SDLK_LCTRL));
   rb_define_const(ray_cEvent, "KEY_RALT", INT2FIX(SDLK_RALT));
   rb_define_const(ray_cEvent, "KEY_LALT", INT2FIX(SDLK_LALT));
   rb_define_const(ray_cEvent, "KEY_RMETA", INT2FIX(SDLK_RMETA));
   rb_define_const(ray_cEvent, "KEY_LMETA", INT2FIX(SDLK_LMETA));
   rb_define_const(ray_cEvent, "KEY_LSUPER", INT2FIX(SDLK_LSUPER));
   rb_define_const(ray_cEvent, "KEY_RSUPER", INT2FIX(SDLK_RSUPER));
   rb_define_const(ray_cEvent, "KEY_MODE", INT2FIX(SDLK_MODE));
   rb_define_const(ray_cEvent, "KEY_COMPOSE", INT2FIX(SDLK_COMPOSE));
   rb_define_const(ray_cEvent, "KEY_HELP", INT2FIX(SDLK_HELP));
   rb_define_const(ray_cEvent, "KEY_PRINT", INT2FIX(SDLK_PRINT));
   rb_define_const(ray_cEvent, "KEY_SYSREQ", INT2FIX(SDLK_SYSREQ));
   rb_define_const(ray_cEvent, "KEY_BREAK", INT2FIX(SDLK_BREAK));
   rb_define_const(ray_cEvent, "KEY_MENU", INT2FIX(SDLK_MENU));
   rb_define_const(ray_cEvent, "KEY_POWER", INT2FIX(SDLK_POWER));
   rb_define_const(ray_cEvent, "KEY_EURO", INT2FIX(SDLK_EURO));
   rb_define_const(ray_cEvent, "KEY_UNDO", INT2FIX(SDLK_UNDO));
   rb_define_const(ray_cEvent, "KMOD_NONE", INT2FIX(KMOD_NONE));
   rb_define_const(ray_cEvent, "KMOD_LCTRL", INT2FIX(KMOD_LCTRL));
   rb_define_const(ray_cEvent, "KMOD_RCTRL", INT2FIX(KMOD_RCTRL));
   rb_define_const(ray_cEvent, "KMOD_LALT", INT2FIX(KMOD_LALT));
   rb_define_const(ray_cEvent, "KMOD_RALT", INT2FIX(KMOD_RALT));
   rb_define_const(ray_cEvent, "KMOD_LMETA", INT2FIX(KMOD_LMETA));
   rb_define_const(ray_cEvent, "KMOD_RMETA", INT2FIX(KMOD_RMETA));
   rb_define_const(ray_cEvent, "KMOD_NUM", INT2FIX(KMOD_NUM));
   rb_define_const(ray_cEvent, "KMOD_CAPS", INT2FIX(KMOD_CAPS));
   rb_define_const(ray_cEvent, "KMOD_MODE", INT2FIX(KMOD_MODE));
   rb_define_const(ray_cEvent, "KMOD_RESERVED", INT2FIX(KMOD_RESERVED));

   rb_define_const(ray_cEvent, "APPMOUSEFOCUS", INT2FIX(SDL_APPMOUSEFOCUS));
   rb_define_const(ray_cEvent, "APPINPUTFOCUS", INT2FIX(SDL_APPINPUTFOCUS));
   rb_define_const(ray_cEvent, "APPACTIVE", INT2FIX(SDL_APPACTIVE));
}
