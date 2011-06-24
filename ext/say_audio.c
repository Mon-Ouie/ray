#include "say.h"

void say_audio_set_volume(float vol) {
  say_audio_context_ensure();
  alListenerf(AL_GAIN, vol / 100);
}

void say_audio_set_pos(say_vector3 pos) {
  say_audio_context_ensure();
  alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
}

void say_audio_set_direction(say_vector3 dir) {
  say_audio_context_ensure();

  float orientation[6] = {
    dir.x, dir.y, dir.z,
    0.0, 1.0, 0.0
  };

  alListenerfv(AL_ORIENTATION, orientation);
}

float say_audio_get_volume() {
  say_audio_context_ensure();

  float volume = 0;
  alGetListenerf(AL_GAIN, &volume);

  return volume * 100;
}

say_vector3 say_audio_get_pos() {
  say_audio_context_ensure();

  say_vector3 pos;
  alGetListener3f(AL_POSITION, &pos.x, &pos.y, &pos.z);

  return pos;
}

say_vector3 say_audio_get_direction() {
  say_audio_context_ensure();

  float orientation[6];
  alGetListenerfv(AL_ORIENTATION, orientation);

  return say_make_vector3(orientation[0], orientation[1], orientation[2]);
}

ALenum say_audio_get_format(size_t channel_count) {
  say_audio_context_ensure();

  switch (channel_count) {
  case 1: return AL_FORMAT_MONO16;
  case 2: return AL_FORMAT_STEREO16;
  case 4: return alGetEnumValue("AL_FORMAT_QUAD16");
  case 6: return alGetEnumValue("AL_FORMAT_51CHN16");
  case 7: return alGetEnumValue("AL_FORMAT_61CHN16");
  case 8: return alGetEnumValue("AL_FORMAT_71CHN16");

  default: return 0;
  }
}
