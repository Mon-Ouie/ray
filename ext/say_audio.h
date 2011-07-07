#ifndef SAY_AUDIO_H_
#define SAY_AUDIO_H_

typedef struct {
  ALuint buf;

  short *samples;
  size_t sample_count;

  float duration;
} say_sound_buffer;

typedef enum {
  SAY_STATUS_STOPPED,
  SAY_STATUS_PAUSED,
  SAY_STATUS_PLAYING
} say_audio_status;

typedef struct {
  ALuint src;
} say_audio_source;

typedef struct {
  say_audio_source *src;
  say_sound_buffer *buf;
} say_sound;

#define SAY_MUSIC_BUF_COUNT 3

typedef struct {
  say_audio_source *src;
  say_thread *thread;

  ALuint buffers[SAY_MUSIC_BUF_COUNT];
  ALuint last_buffer;

  bool continue_running;
  bool streaming;

  bool looping;

  SF_INFO info;
  SNDFILE *file;

  ALenum format;

  float duration;
  float played_time;
} say_music;

typedef struct {
  void *buf;
  sf_count_t size;
  sf_count_t pos;
} say_vfile;

#define say_make_vfile(str, len) ((say_vfile){str, len, 0})

#define say_setup_vio(io)                                  \
  io.get_filelen = (sf_vio_get_filelen)say_vfile_get_size; \
  io.seek        = (sf_vio_seek)say_vfile_seek;            \
  io.read        = (sf_vio_read)say_vfile_read;            \
  io.write       = (sf_vio_write)say_vfile_write;          \
  io.tell        = (sf_vio_tell)say_vfile_tell;

/* Audio context */

void say_audio_context_ensure();
void say_audio_context_clean_up();

/* Audio */

void say_audio_set_volume(float vol);
void say_audio_set_pos(say_vector3 pos);
void say_audio_set_direction(say_vector3 dir);

float say_audio_get_volume();
say_vector3 say_audio_get_pos();
say_vector3 say_audio_get_direction();

ALenum say_audio_get_format(size_t channel_count);

/* Sound buffers */

say_sound_buffer *say_sound_buffer_create();
void say_sound_buffer_free(say_sound_buffer *buf);

int say_sound_buffer_load_from_memory(say_sound_buffer *buf, size_t len,
                                       const char *str);
int say_sound_buffer_load_from_file(say_sound_buffer *buf, const char *filename);

short *say_sound_buffer_get_samples(say_sound_buffer *buf);
size_t say_sound_buffer_get_sample_count(say_sound_buffer *buf);
size_t say_sound_buffer_get_channel_count(say_sound_buffer *buf);
size_t say_sound_buffer_get_sample_rate(say_sound_buffer *buf);
float say_sound_buffer_get_duration(say_sound_buffer *buf);

/* Audio sources */

say_audio_source *say_audio_source_create();
void say_audio_source_free(say_audio_source *src);

void say_audio_source_set_pitch(say_audio_source *src, float pitch);
float say_audio_source_get_pitch(say_audio_source *src);

void say_audio_source_set_volume(say_audio_source *src, float vol);
float say_audio_source_get_volume(say_audio_source *src);

void say_audio_source_set_pos(say_audio_source *src, say_vector3 pos);
say_vector3 say_audio_source_get_pos(say_audio_source *src);

void say_audio_source_set_relative(say_audio_source *src, uint8_t rel);
uint8_t say_audio_source_get_relative(say_audio_source *src);

void say_audio_source_set_min_distance(say_audio_source *src, float dist);
float say_audio_source_get_min_distance(say_audio_source *src);

void say_audio_source_set_attenuation(say_audio_source *src, float att);
float say_audio_source_get_attenuation(say_audio_source *src);

say_audio_status say_audio_source_get_status(say_audio_source *src);

/* Sounds */

say_sound *say_sound_create();
void say_sound_free(say_sound *snd);

void say_sound_set_buffer(say_sound *snd, say_sound_buffer *buf);
say_sound_buffer *say_sound_get_buffer(say_sound *snd);

void say_sound_set_looping(say_sound *snd, uint8_t val);
uint8_t say_sound_is_looping(say_sound *snd);

void say_sound_seek(say_sound *snd, float time);
float say_sound_get_time(say_sound *snd);

float say_sound_get_duration(say_sound *snd);

void say_sound_play(say_sound *snd);
void say_sound_pause(say_sound *snd);
void say_sound_stop(say_sound *snd);

/* Virtual files */

sf_count_t say_vfile_get_size(say_vfile *file);
sf_count_t say_vfile_seek(sf_count_t offset, int whence, say_vfile *file);
sf_count_t say_vfile_read(void *buf, sf_count_t count, say_vfile *file);
sf_count_t say_vfile_write(const void *buf, sf_count_t count, say_vfile *file);
sf_count_t say_vfile_tell(say_vfile *file);

/* Musics */

say_music *say_music_create();
void say_music_free(say_music *music);

bool say_music_open(say_music *music, const char *filename);

void say_music_set_looping(say_music *music, bool val);
bool say_music_is_looping(say_music *music);

void say_music_seek(say_music *music, float time);
float say_music_get_time(say_music *music);

float say_music_get_duration(say_music *music);

void say_music_play(say_music *music);
void say_music_pause(say_music *music);
void say_music_stop(say_music *music);

#endif
