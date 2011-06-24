#include "say.h"

say_audio_source *say_audio_source_create() {
  say_audio_context_ensure();

  say_audio_source *src = malloc(sizeof(say_audio_source));
  alGenSources(1, &src->src);

  return src;
}

void say_audio_source_free(say_audio_source *src) {
  alSourcei(src->src, AL_BUFFER, 0);
  alDeleteSources(1, &src->src);
  free(src);
}

void say_audio_source_set_pitch(say_audio_source *src, float pitch) {
  alSourcef(src->src, AL_PITCH, pitch);
}

float say_audio_source_get_pitch(say_audio_source *src) {
  float pitch;
  alGetSourcef(src->src, AL_PITCH, &pitch);

  return pitch;
}

void say_audio_source_set_volume(say_audio_source *src, float vol) {
  alSourcef(src->src, AL_GAIN, vol / 100);
}

float say_audio_source_get_volume(say_audio_source *src) {
  float vol;
  alGetSourcef(src->src, AL_GAIN, &vol);

  return vol * 100;
}

void say_audio_source_set_pos(say_audio_source *src, say_vector3 pos) {
  alSource3f(src->src, AL_POSITION, pos.x, pos.y, pos.z);
}

say_vector3 say_audio_source_get_pos(say_audio_source *src) {
  say_vector3 pos;
  alGetSource3f(src->src, AL_POSITION, &pos.x, &pos.y, &pos.z);

  return pos;
}

void say_audio_source_set_relative(say_audio_source *src, uint8_t rel) {
  alSourcei(src->src, AL_SOURCE_RELATIVE, rel);
}

uint8_t say_audio_source_get_relative(say_audio_source *src) {
  ALint rel;
  alGetSourcei(src->src, AL_SOURCE_RELATIVE, &rel);

  return rel;
}

void say_audio_source_set_min_distance(say_audio_source *src, float dist) {
  alSourcef(src->src, AL_REFERENCE_DISTANCE, dist);
}


float say_audio_source_get_min_distance(say_audio_source *src) {
  ALfloat dist;
  alGetSourcef(src->src, AL_REFERENCE_DISTANCE, &dist);

  return dist;
}

void say_audio_source_set_attenuation(say_audio_source *src, float att) {
  alSourcef(src->src, AL_ROLLOFF_FACTOR, att);
}

float say_audio_source_get_attenuation(say_audio_source *src) {
  ALfloat att;
  alGetSourcef(src->src, AL_ROLLOFF_FACTOR, &att);

  return att;
}

say_audio_status say_audio_source_get_status(say_audio_source *src) {
  ALint status;
  alGetSourcei(src->src, AL_SOURCE_STATE, &status);

  switch (status) {
  case AL_INITIAL: return SAY_STATUS_STOPPED;
  case AL_STOPPED: return SAY_STATUS_STOPPED;
  case AL_PAUSED:  return SAY_STATUS_PAUSED;
  case AL_PLAYING: return SAY_STATUS_PLAYING;
  default:         return SAY_STATUS_STOPPED;
  }
}
