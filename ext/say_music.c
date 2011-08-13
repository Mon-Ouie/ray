#include "say.h"

static size_t say_music_get_buf_size(say_music *music) {
  return music->info.channels * music->info.samplerate;
}

static sf_count_t say_music_read_chunk(say_music *music, short *buf) {
  sf_count_t size = (sf_count_t)say_music_get_buf_size(music);
  return sf_read_short(music->file, buf, size);
}

static bool say_music_fill_buffer(say_music *music, ALint al_buffer) {
  sf_count_t sample_count = say_music_get_buf_size(music);

  size_t buf_size = sizeof(short) * sample_count;
  short *buf = malloc(buf_size);

  sf_count_t read_samples = say_music_read_chunk(music, buf);
  alBufferData(al_buffer, music->format, buf, read_samples * sizeof(short),
               music->info.samplerate);

  free(buf);

  return read_samples != 0;
}

static void say_music_start_stream(say_music *music) {
  ALint queued = 0;
  alGetSourcei(music->src->src, AL_BUFFERS_QUEUED, &queued);

  while (queued > 0) {
    queued--;

    ALuint buf;
    alSourceUnqueueBuffers(music->src->src, 1, &buf);
  }

  for (size_t i = 0; i < SAY_MUSIC_BUF_COUNT; i++) {
    say_music_fill_buffer(music, music->buffers[i]);
    alSourceQueueBuffers(music->src->src, 1, &music->buffers[i]);
    music->last_buffer = music->buffers[i];
  }
}

static void *say_music_playback_thread(say_music *music) {
  while (music->continue_running) {
    if (music->streaming) {
      ALint processed = 0;
      alGetSourcei(music->src->src, AL_BUFFERS_PROCESSED, &processed);

      while (processed > 0) {
        processed--;

        ALuint buffer;
        alSourceUnqueueBuffers(music->src->src, 1, &buffer);

        /* compute duration of the buffer */
        ALint size, chan, freq;
        alGetBufferi(buffer, AL_SIZE, &size);
        alGetBufferi(buffer, AL_FREQUENCY, &freq);
        alGetBufferi(buffer, AL_CHANNELS, &chan);

        /* adds 1 for any buffer but the last one */
        music->played_time += (float)size / (freq * chan * sizeof(short));

        bool still_playing = say_music_fill_buffer(music, buffer);
        if (!still_playing) {
          if (music->looping && music->last_buffer == buffer) {
            say_music_seek(music, 0);
          }
          else if (!music->looping) {
            music->streaming = false;
          }
        }
        else {
          alSourceQueueBuffers(music->src->src, 1, &buffer);
          music->last_buffer = buffer;
        }
      }
    }

    /* sleep for 0.25s */
#ifdef SAY_WIN
    Sleep(250);
#else
    usleep(250 * 1000);
#endif
  }

  return NULL;
}

say_music *say_music_create() {
  say_music *music = malloc(sizeof(say_music));

  music->src = say_audio_source_create();
  alGenBuffers(SAY_MUSIC_BUF_COUNT, music->buffers);

  music->last_buffer = 0;

  music->file = NULL;

  music->duration = 0;

  music->continue_running = true;
  music->streaming        = false;

  music->looping = false;

  music->thread = say_thread_create(music,
                                    (say_thread_func)say_music_playback_thread);

  return music;
}

void say_music_free(say_music *music) {
  music->continue_running = false;

#ifndef SAY_WIN
  say_thread_join(music->thread);
#endif
  say_thread_free(music->thread);

  alSourceStop(music->src->src);
  alDeleteBuffers(SAY_MUSIC_BUF_COUNT, music->buffers);

  say_audio_source_free(music->src);

  if (music->file)
    sf_close(music->file);

  free(music);
}

bool say_music_open(say_music *music, const char *filename) {
  music->streaming = false;
  alSourceStop(music->src->src);

  if (music->file) {
    sf_close(music->file);
  }

  music->file = sf_open(filename, SFM_READ, &music->info);

  if (!music->file) {
    say_error_set(sf_strerror(music->file));
    return false;
  }

  music->format = say_audio_get_format(music->info.channels);
  if (!music->format) {
    sf_close(music->file);
    music->file = NULL;

    say_error_set("unsupported amount of channels");
    return false;
  }

  music->duration = (float)music->info.frames / music->info.samplerate;
  music->played_time = 0;

  return true;
}

void say_music_set_looping(say_music *music, bool val) {
  music->looping = val;
}

bool say_music_is_looping(say_music *music) {
  return music->looping;
}

void say_music_seek(say_music *music, float time) {
  if (music->file) {
    music->played_time = time;
    sf_seek(music->file, music->info.samplerate * time, SEEK_SET);

    if (music->streaming) {
      music->streaming = false;
      alSourceStop(music->src->src);

      say_music_start_stream(music);

      alSourcePlay(music->src->src);
      music->streaming = true;
    }
  }
}

float say_music_get_time(say_music *music) {
  ALfloat time = 0.0f;
  alGetSourcef(music->src->src, AL_SEC_OFFSET, &time);

  return time + music->played_time;
}

float say_music_get_duration(say_music *music) {
  return music->duration;
}

void say_music_play(say_music *music) {
  if (music->file) {
    /* queue more data if sound was stopped */
    if (say_audio_source_get_status(music->src) == SAY_STATUS_STOPPED) {
      say_music_seek(music, 0);
      say_music_start_stream(music);
    }

    alSourcePlay(music->src->src);
    music->streaming = true;
  }
}

void say_music_pause(say_music *music) {
  if (music->file) {
    music->streaming = false;
    alSourcePause(music->src->src);
  }
}

void say_music_stop(say_music *music) {
  if (music->file) {
    music->streaming = false;
    say_music_seek(music, 0);
    alSourceStop(music->src->src);
  }
}
