#include "say.h"

say_sound *say_sound_create() {
  say_sound *snd = malloc(sizeof(say_sound));

  snd->src = say_audio_source_create();
  snd->buf = NULL;

  return snd;
}

void say_sound_free(say_sound *snd) {
  say_audio_source_free(snd->src);
  free(snd);
}

void say_sound_set_buffer(say_sound *snd, say_sound_buffer *buf) {
  say_sound_stop(snd);

  snd->buf = buf;
  alSourcei(snd->src->src, AL_BUFFER, buf ? buf->buf : 0);
}

say_sound_buffer *say_sound_get_buffer(say_sound *snd) {
  return snd->buf;
}

void say_sound_set_looping(say_sound *snd, uint8_t val) {
  alSourcei(snd->src->src, AL_LOOPING, val);
}

uint8_t say_sound_is_looping(say_sound *snd) {
  ALint looping;
  alGetSourcei(snd->src->src, AL_LOOPING, &looping);

  return looping;
}

void say_sound_seek(say_sound *snd, float time) {
  alSourcef(snd->src->src, AL_SEC_OFFSET, time);
}

float say_sound_get_time(say_sound *snd) {
  float time;
  alGetSourcef(snd->src->src, AL_SEC_OFFSET, &time);

  return time;
}

float say_sound_get_duration(say_sound *snd) {
  if (snd->buf)
    return say_sound_buffer_get_duration(snd->buf);
  else
    return 0;
}

void say_sound_play(say_sound *snd) {
  alSourcePlay(snd->src->src);
}

void say_sound_pause(say_sound *snd) {
  alSourcePause(snd->src->src);
}

void say_sound_stop(say_sound *snd) {
  alSourceStop(snd->src->src);
}
