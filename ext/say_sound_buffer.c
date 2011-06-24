#include "say.h"

say_sound_buffer *say_sound_buffer_create() {
  say_sound_buffer *buf = malloc(sizeof(say_sound_buffer));

  say_audio_context_ensure();
  alGenBuffers(1, &buf->buf);

  buf->samples      = NULL;
  buf->sample_count = 0;

  buf->duration = 0;

  return buf;
}

void say_sound_buffer_free(say_sound_buffer *buf) {
  if (buf->samples)
    free(buf->samples);

  alDeleteBuffers(1, &buf->buf);

  free(buf);
}

static int say_sound_buffer_load(say_sound_buffer *buf, SF_INFO info, SNDFILE *file) {
  if (!file) {
    say_error_set(sf_strerror(file));
    return 0;
  }

  buf->sample_count = info.frames * info.channels;
  buf->samples = realloc(buf->samples, sizeof(short) * buf->sample_count);

  sf_read_short(file, buf->samples, buf->sample_count);

  sf_close(file);

  ALenum fmt = say_audio_get_format(info.channels);
  if (!fmt) {
    say_error_set("unsupported amount of channels");
    return 0;
  }

  alBufferData(buf->buf, fmt, buf->samples, buf->sample_count * sizeof(short),
               info.samplerate);

  buf->duration = (float)buf->sample_count / info.samplerate / info.channels;

  return 1;
}

int say_sound_buffer_load_from_memory(say_sound_buffer *buf, size_t len,
                                      const char *str) {
  SF_INFO info;
  say_vfile vfile = say_make_vfile((void*)str, len);

  SF_VIRTUAL_IO vio;
  say_setup_vio(vio);

  SNDFILE *file = sf_open_virtual(&vio, SFM_READ, &info, &vfile);

  return say_sound_buffer_load(buf, info, file);
}

int say_sound_buffer_load_from_file(say_sound_buffer *buf, const char *filename) {
  SF_INFO info;
  SNDFILE *file = sf_open(filename, SFM_READ, &info);
  return say_sound_buffer_load(buf, info, file);
}

short *say_sound_buffer_get_samples(say_sound_buffer *buf) {
  return buf->samples;
}

size_t say_sound_buffer_get_sample_count(say_sound_buffer *buf) {
  return buf->sample_count;
}

size_t say_sound_buffer_get_channel_count(say_sound_buffer *buf) {
  ALint channel_count;
  alGetBufferi(buf->buf, AL_CHANNELS, &channel_count);

  return channel_count;
}

size_t say_sound_buffer_get_sample_rate(say_sound_buffer *buf) {
  ALint sample_rate;
  alGetBufferi(buf->buf, AL_FREQUENCY, &sample_rate);

  return sample_rate;
}

float say_sound_buffer_get_duration(say_sound_buffer *buf) {
  return buf->duration;
}
