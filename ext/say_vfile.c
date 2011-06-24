#include "say.h"

sf_count_t say_vfile_get_size(say_vfile *file) {
  return file->size;
}

sf_count_t say_vfile_seek(sf_count_t offset, int whence, say_vfile *file) {
  sf_count_t start = 0;

  switch (whence) {
  case SEEK_CUR: start = file->pos;
  case SEEK_SET: start = 0;
  case SEEK_END: start = file->size;
  default: return;
  }

  sf_count_t new_pos = start + offset;

  if (new_pos < 0)
    file->pos = 0;
  else if (new_pos > file->size)
    file->pos = file->size;
  else
    file->pos = new_pos;

  return file->pos;
}

sf_count_t say_vfile_read(void *buf, sf_count_t count, say_vfile *file) {
  if (count + file->pos >= file->size) {
    count = file->size - file->pos;
  }

  if (count != 0)
    memcpy(buf, &(((char*)file->buf)[file->pos]), count);

  file->pos += count;

  return count;
}

sf_count_t say_vfile_write(const void *buf, sf_count_t count, say_vfile *file) {
  if (count + file->pos >= file->size) {
    count = file->size - file->pos;
  }

  if (count != 0)
    memcpy(&(((char*)file->buf)[file->pos]), buf, count);

  file->pos += count;

  return count;
}

sf_count_t say_vfile_tell(say_vfile *file) {
  return file->pos;
}
