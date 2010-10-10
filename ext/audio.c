#include "ray.h"

#ifdef HAVE_SDL_MIXER

VALUE ray_mAudio = Qnil;
VALUE ray_cSound = Qnil;
VALUE ray_cMusic = Qnil;

/* @return [Integer] The audio frequency */
VALUE ray_audio_frequency() {
   int freq = 0, format = 0, channels = 0;
   Mix_QuerySpec(&freq, (uint16_t*)&format, &channels);

   return INT2FIX(freq);
}

/* @return [Integer] The audio format */
VALUE ray_audio_format() {
   int freq = 0, format = 0, channels = 0;
   Mix_QuerySpec(&freq, (uint16_t*)&format, &channels);

   return INT2FIX(format);
}

/* @return [true, false] true if mono is used */
VALUE ray_audio_mono() {
   int freq = 0, format = 0, channels = 0;
   Mix_QuerySpec(&freq, (uint16_t*)&format, &channels);

   return channels == 1 ? Qtrue : Qfalse;
}

/* @return [true, false] true if stereo is used */
VALUE ray_audio_stereo() {
   int freq = 0, format = 0, channels = 0;
   Mix_QuerySpec(&freq, (uint16_t*)&format, &channels);

   return channels == 1 ? Qfalse : Qtrue;
}

/* @return [Float] The volume for the song which is currently being played */ 
VALUE ray_audio_volume(VALUE self) {
   int volume = Mix_VolumeMusic(-1);
   return rb_float_new((volume / 128.0f) * 100.0f);
}

/* Sets the volume of the song which is currently being played */
VALUE ray_audio_set_volume(VALUE self, VALUE value) {
   float volume = (float)NUM2DBL(value);
   Mix_VolumeMusic((int)((volume / 100.0f) * 128.0f));

   return value;
}

/*
  @overload playing?(channel = nil)
    @return [true, false] If channel is set, true if something is playing on
      that channel. If it isn't, true if a music is currently being played.
 */
VALUE ray_audio_playing(int argc, VALUE *argv, VALUE self) {
   VALUE channel = Qnil;
   rb_scan_args(argc, argv, "01", &channel);

   if (NIL_P(channel))
      return Mix_PlayingMusic() ? Qtrue : Qfalse;
   return Mix_Playing(NUM2INT(channel)) ? Qtrue: Qfalse;
}

/* 
   @overload stop(channel = nil)
     If channel is set, stops playback on a channel. Stops playing
     the current music otherwise.
*/
VALUE ray_audio_stop(int argc, VALUE *argv, VALUE self) {
   VALUE channel;
   rb_scan_args(argc, argv, "01", &channel);

   if (!NIL_P(channel)) {
      Mix_HaltChannel(NUM2INT(channel));
      return channel;
   }

   Mix_HaltMusic();
   return Qnil;
}

/*
  @overload pause(channel = nil)
    If channel is set, pauses a channel. Pauses the current music otherwise.
*/
VALUE ray_audio_pause(int argc, VALUE *argv, VALUE self) {
   VALUE channel;
   rb_scan_args(argc, argv, "01", &channel);

   if (!NIL_P(channel)) {
      Mix_Pause(NUM2INT(channel));
      return channel;
   }

   Mix_PauseMusic();
   return Qnil;
}

/*
  @overload resume(channel = nil)
   If channel is set, resumes a paused channel from pause. Resumes the music
   otherwise.
*/
VALUE ray_audio_resume(int argc, VALUE *argv, VALUE self) {
   VALUE channel;
   rb_scan_args(argc, argv, "01", &channel);

   if (!NIL_P(channel)) {
      Mix_Resume(NUM2INT(channel));
      return channel;
   }

   Mix_ResumeMusic();
   return Qnil;
}

/*
  @return [true, false] True if the channel is paused. If channel isn't set,
    returns true if the current music isn't paused.
 */
VALUE ray_audio_paused(int argc, VALUE *argv, VALUE self) {
   VALUE channel;
   rb_scan_args(argc, argv, "01", &channel);

   if (!NIL_P(channel))
      return Mix_Paused(NUM2INT(channel)) ? Qtrue : Qfalse;
   return Mix_PausedMusic() ? Qtrue : Qfalse;
}

/*
  Sets the position in the current music. May not be implemented for some
  formats.  
*/
VALUE ray_audio_set_music_pos(VALUE self, VALUE val) {
	Mix_SetMusicPosition(NUM2DBL(val));
	return val;
}

Mix_Chunk *ray_rb2chunk(VALUE object) {
   if (!RAY_IS_A(object, ray_cSound)) {
      rb_raise(rb_eTypeError, "Can't convert %s into Ray::Sound",
               RAY_OBJ_CLASSNAME(object));
   }

   ray_sound *ptr = NULL;
   Data_Get_Struct(object, ray_sound, ptr);

   return ptr->sound;
}

void ray_free_sound(ray_sound *sound) {
   if (sound->sound) Mix_FreeChunk(sound->sound);
   free(sound);
}

VALUE ray_alloc_sound(VALUE self) {
   ray_sound *ptr = malloc(sizeof(ray_sound));
   ptr->sound = NULL;

   VALUE ret = Data_Wrap_Struct(self, 0, ray_free_sound, ptr);

   return ret;
}

void ray_init_sound_with_filename(VALUE self, VALUE filename) {
   char *c_filename = StringValuePtr(filename);
   ray_sound *sound = NULL;
   Data_Get_Struct(self, ray_sound, sound);

   sound->sound = Mix_LoadWAV(c_filename);
   if (!sound->sound) {
      rb_raise(rb_eRuntimeError, "Could not load sound (%s)",
               Mix_GetError());
   }
}

void ray_init_sound_with_io(VALUE self, VALUE io) {
   ray_sound *sound = NULL;
   Data_Get_Struct(self, ray_sound, sound);

   VALUE string  = rb_funcall2(io, RAY_METH("read"), 0, NULL);
   char *content = StringValuePtr(string);
   SDL_RWops *data = SDL_RWFromMem(content, (int)RSTRING_LEN(string));

   if (!data) {
      rb_raise(rb_eRuntimeError, "Could not create music data (%s)",
               SDL_GetError());
   }
   
   sound->sound = Mix_LoadWAV_RW(data, 1);
   if (!sound->sound) {
      rb_raise(rb_eRuntimeError, "Could not load sound (%s)",
               Mix_GetError());
   }
}

/*
  Creates a new sound from an IO object or a filename.
*/
VALUE ray_init_sound(VALUE self, VALUE arg) {
   if (rb_respond_to(arg, RAY_METH("to_str")))
      ray_init_sound_with_filename(self, rb_String(arg));
   else if (rb_respond_to(arg, RAY_METH("read")))
      ray_init_sound_with_io(self, arg);
   else {
      rb_raise(rb_eTypeError, "Can't convert %s into String",
               RAY_OBJ_CLASSNAME(arg));
   }

   return Qnil;
}

/*
  @overload play(channel = 0, times = 1)
    Plays a sound on the given channel a given number of times.
    @param [Integer, :forever] tiems How many times the song should be played.
      Can also be :forever or 0 to play it forever.
 */
VALUE ray_sound_play(int argc, VALUE *argv, VALUE self) {
   VALUE channel, times;
   rb_scan_args(argc, argv, "02", &channel, &times);

   if (NIL_P(channel)) channel = INT2FIX(0);
   if (NIL_P(times))   times   = INT2FIX(1);

   int c_channel = NUM2INT(channel), c_times = 0;
   if (times == RAY_SYM("forever"))
      c_times = 0;
   else
      c_times = NUM2INT(times);

   Mix_Chunk *chunk = ray_rb2chunk(self);
   Mix_PlayChannel(c_channel, chunk, c_times - 1);

   return self;
}

/*
  @overload fade(duration, channel = 0, times = 1)
    Same as play, but fades for a given number of a seconds.
    @param [Float] duration Duration of the fade, in seconds.
    @see Ray::Sound#play
 */
VALUE ray_sound_fade(int argc, VALUE *argv, VALUE self) {
   VALUE duration, channel, times;
   rb_scan_args(argc, argv, "12", &duration, &channel, &times);

   if (NIL_P(channel)) channel = INT2FIX(0);
   if (NIL_P(times))   times   = INT2FIX(1);

   int c_channel = NUM2INT(channel), c_times = 0;
   if (times == RAY_SYM("forever"))
      c_times = 0;
   else
      c_times = NUM2INT(times);

   Mix_Chunk *chunk = ray_rb2chunk(self);
   Mix_FadeInChannel(c_channel, chunk, c_times - 1,
                     (int)(NUM2DBL(duration) * 1000));

   return self;
}

/* @return [Float] Volume of the sound, between 0 and 100. */
VALUE ray_sound_volume(VALUE self) {
   Mix_Chunk *chunk = ray_rb2chunk(self);
   int volume = Mix_VolumeChunk(chunk, -1);

   return rb_float_new((volume / 128.0f) * 100.0f);
}

/* Sets the volume of the sound. */
VALUE ray_sound_set_volume(VALUE self, VALUE value) {
   Mix_Chunk *chunk = ray_rb2chunk(self);
   float volume = (float)NUM2DBL(value);
   Mix_VolumeChunk(chunk, (int)((volume / 100.0f) * 128.0f));

   return value;
}

Mix_Music *ray_rb2music(VALUE object) {
   if (!RAY_IS_A(object, ray_cMusic)) {
      rb_raise(rb_eTypeError, "Can't convert %s into Ray::Music",
               RAY_OBJ_CLASSNAME(object));
   }

   ray_music *ptr = NULL;
   Data_Get_Struct(object, ray_music, ptr);

   return ptr->music;
}

void ray_free_music(ray_music *music) {
   if (music->music) Mix_FreeMusic(music->music);
   free(music);
}

VALUE ray_alloc_music(VALUE self) {
   ray_music *ptr = malloc(sizeof(ray_music));
   ptr->music = NULL;

   VALUE ret = Data_Wrap_Struct(self, 0, ray_free_music, ptr);

   return ret;
}

void ray_init_music_with_filename(VALUE self, VALUE filename) {
   char *c_filename = StringValuePtr(filename);
   ray_music *music = NULL;
   Data_Get_Struct(self, ray_music, music);

   music->music = Mix_LoadMUS(c_filename);
   if (!music->music) {
      rb_raise(rb_eRuntimeError, "Could not load music (%s)",
               Mix_GetError());
   }
}

void ray_init_music_with_io(VALUE self, VALUE io) {
   ray_music *music = NULL;
   Data_Get_Struct(self, ray_music, music);

   VALUE string  = rb_funcall2(io, RAY_METH("read"), 0, NULL);
   char *content = StringValuePtr(string);
   SDL_RWops *data = SDL_RWFromMem(content, (int)RSTRING_LEN(string));

   if (!data) {
      rb_raise(rb_eRuntimeError, "Could not create music data (%s)",
               SDL_GetError());
   }
   
   music->music = Mix_LoadMUS_RW(data);
   if (!music->music) {
      rb_raise(rb_eRuntimeError, "Could not load music (%s)",
               Mix_GetError());
   }

   SDL_FreeRW(data);
}

/*
  Creates a new music from an IO object or a filename.
*/
VALUE ray_init_music(VALUE self, VALUE arg) {
   if (rb_respond_to(arg, RAY_METH("to_str")))
      ray_init_music_with_filename(self, rb_String(arg));
   else if (rb_respond_to(arg, RAY_METH("read")))
      ray_init_music_with_io(self, arg);
   else {
      rb_raise(rb_eTypeError, "Can't convert %s into String",
               RAY_OBJ_CLASSNAME(arg));
   }

   return Qnil;
}

/* @return [Integer] The type of the music */
VALUE ray_music_type(VALUE self) {
   Mix_Music *music = ray_rb2music(self);
   Mix_MusicType ret = Mix_GetMusicType(music);
   
   return INT2FIX(ret);
}

/*
  @overload play(times = 1)
    
 */
VALUE ray_music_play(int argc, VALUE *argv, VALUE self) {
   VALUE times;
   rb_scan_args(argc, argv, "01", &times);

   if (NIL_P(times)) times = INT2FIX(1);

   int c_times = 0;
   if (times == RAY_SYM("forever"))
      c_times = 0;
   else
      c_times = NUM2INT(times);

   Mix_Music *music = ray_rb2music(self);
   Mix_PlayMusic(music, c_times);

   return self;
}

/*
  Document-class: Ray::Music

  Class used to play music. Notice only one music can be played at the
  same time.
*/

/*
  Document-class: Ray::Sound

  Class used to represent short sounds. You can play one sound per channel.
*/

void Init_ray_audio() {
   ray_mAudio = rb_define_module_under(ray_mRay, "Audio");
      
   rb_define_const(ray_mAudio, "FORMAT_U8",     INT2FIX(AUDIO_U8));
   rb_define_const(ray_mAudio, "FORMAT_S8",     INT2FIX(AUDIO_S8));
   rb_define_const(ray_mAudio, "FORMAT_U16LSB", INT2FIX(AUDIO_U16LSB));
   rb_define_const(ray_mAudio, "FORMAT_S16LSB", INT2FIX(AUDIO_S16LSB));
   rb_define_const(ray_mAudio, "FORMAT_U16MSB", INT2FIX(AUDIO_U16MSB));
   rb_define_const(ray_mAudio, "FORMAT_S16MSB", INT2FIX(AUDIO_S16MSB));
   rb_define_const(ray_mAudio, "FORMAT_U16",    INT2FIX(AUDIO_U16));
   rb_define_const(ray_mAudio, "FORMAT_S16",    INT2FIX(AUDIO_S16));
   rb_define_const(ray_mAudio, "FORMAT_U16SYS", INT2FIX(AUDIO_U16SYS));
   rb_define_const(ray_mAudio, "FORMAT_S16SYS", INT2FIX(AUDIO_S16SYS));

   rb_define_module_function(ray_mAudio, "frequency", ray_audio_frequency, 0);
   rb_define_module_function(ray_mAudio, "format", ray_audio_format, 0);
   rb_define_module_function(ray_mAudio, "mono?", ray_audio_mono, 0);
   rb_define_module_function(ray_mAudio, "stereo?", ray_audio_stereo, 0);

   rb_define_module_function(ray_mAudio, "volume", ray_audio_volume, 0);
   rb_define_module_function(ray_mAudio, "volume=", ray_audio_set_volume, 1);

   rb_define_module_function(ray_mAudio, "stop", ray_audio_stop, -1);
   rb_define_module_function(ray_mAudio, "pause", ray_audio_pause, -1);
   rb_define_module_function(ray_mAudio, "resume", ray_audio_resume, -1);
   rb_define_module_function(ray_mAudio, "paused?", ray_audio_paused, -1);
   rb_define_module_function(ray_mAudio, "playing?", ray_audio_playing, -1);

	rb_define_module_function(ray_mAudio, "music_pos=", ray_audio_set_music_pos, 1);

   ray_cSound = rb_define_class_under(ray_mRay, "Sound", rb_cObject);
   rb_define_alloc_func(ray_cSound, ray_alloc_sound);
   rb_define_method(ray_cSound, "initialize", ray_init_sound, 1);
   rb_define_method(ray_cSound, "play", ray_sound_play, -1);
   rb_define_method(ray_cSound, "fade", ray_sound_fade, -1);
   rb_define_method(ray_cSound, "volume", ray_sound_volume, 0);
   rb_define_method(ray_cSound, "volume=", ray_sound_set_volume, 1);
   
   ray_cMusic = rb_define_class_under(ray_mRay, "Music", rb_cObject);
   rb_define_alloc_func(ray_cMusic, ray_alloc_music);
   rb_define_method(ray_cMusic, "initialize", ray_init_music, 1);
   rb_define_method(ray_cMusic, "type", ray_music_type, 0);
   rb_define_method(ray_cMusic, "play", ray_music_play, -1);

   rb_define_const(ray_cMusic, "TYPE_NONE", INT2FIX(MUS_NONE));
   rb_define_const(ray_cMusic, "TYPE_CMD", INT2FIX(MUS_CMD));
   rb_define_const(ray_cMusic, "TYPE_WAV", INT2FIX(MUS_WAV));
   rb_define_const(ray_cMusic, "TYPE_MOD", INT2FIX(MUS_MOD));
   rb_define_const(ray_cMusic, "TYPE_MID", INT2FIX(MUS_MID));
   rb_define_const(ray_cMusic, "TYPE_OGG", INT2FIX(MUS_OGG));
   rb_define_const(ray_cMusic, "TYPE_MP3", INT2FIX(MUS_MP3));
#ifndef PSP
   rb_define_const(ray_cMusic, "TYPE_MP3_MAD", INT2FIX(MUS_MP3_MAD));
   rb_define_const(ray_cMusic, "TYPE_FLAC", INT2FIX(MUS_FLAC));
#endif
}

#endif
