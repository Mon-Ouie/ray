#include "say.h"

#define SAY_MAX_SIZE        ((size_t)-1)
#define SAY_BUFFER_MAX_SIZE ((4 * 1024 * 1024) / sizeof(GLuint))
#define SAY_BUFFER_MIN_SIZE ((4 * 1024) / sizeof(GLuint))

typedef struct {
  size_t loc, size;
} say_range;

#define say_make_range(begin, size) ((say_range){begin, size})

typedef struct {
  say_index_buffer *buf;
  say_array        *ranges;
} say_global_ibo;

static say_array *say_index_buffers = NULL;

static say_global_ibo say_global_ibo_create(size_t size) {
  say_global_ibo ret;
  ret.buf    = say_index_buffer_create(SAY_STREAM, size);
  ret.ranges = say_array_create(sizeof(say_range), NULL, NULL);

  return ret;
}

static void say_global_ibo_free(void *data) {
  say_global_ibo *ibo = (say_global_ibo*)data;

  say_index_buffer_free(ibo->buf);
  say_array_free(ibo->ranges);
}

static say_global_ibo *say_global_ibo_at(size_t index) {
  if (!say_index_buffers)
    return NULL;
  return say_array_get(say_index_buffers, index);
}

static size_t say_global_ibo_insert(say_global_ibo *ibo, size_t i,
                                    size_t size) {
  say_range tmp = say_make_range(0, size);
  say_array_insert(ibo->ranges, i, &tmp);

  say_range *range = say_array_get(ibo->ranges, i);

  if (i != 0) {
    say_range *prev = say_array_get(ibo->ranges, i - 1);
    range->loc = prev->loc + prev->size;
  }

  return range->loc;
}

static size_t say_global_ibo_find_in(say_global_ibo *ibo, size_t size) {
  size_t buffer_size = say_index_buffer_get_size(ibo->buf);

  size_t     ary_size = say_array_get_size(ibo->ranges);
  say_range *first    = say_array_get(ibo->ranges, 0);

  /* There's room at the begin of the buffer */
  if ((ary_size == 0 && buffer_size >= size) || (first && first->loc >= size)) {
    return say_global_ibo_insert(ibo, 0, size);
  }

  say_range *current = first, *next = NULL;

  for (size_t i = 0; first && i < ary_size - 1; i++) {
    next = say_array_get(ibo->ranges, i + 1);

    size_t begin = current->loc + current->size;
    size_t end   = next->loc;

    /* There's enough room between those two elements */
    if (end - begin >= size) {
      return say_global_ibo_insert(ibo, i + 1, size);
    }

    current = next;
  }

  say_range *last = say_array_get(ibo->ranges, ary_size - 1);

  /* There's enough room at the end of the buffer */
  if (last && (last->loc + last->size + size) <
      say_index_buffer_get_size(ibo->buf)) {
    return say_global_ibo_insert(ibo, ary_size, size);
  }

  /* Not enough room here. But perhaps we can make some? */
  if (buffer_size < SAY_BUFFER_MAX_SIZE && size <= SAY_BUFFER_MAX_SIZE) {
    size_t sought_size = last ? last->loc + last->size + size : size;
    size_t right_size  = say_index_buffer_get_size(ibo->buf);

    while (right_size < sought_size)
      right_size *= 2;

    say_index_buffer_resize(ibo->buf, right_size);
    return say_global_ibo_insert(ibo, ary_size, size);
  }
  else
    return SAY_MAX_SIZE;
}

static size_t say_global_ibo_find(size_t size, size_t *buf_id) {
  if (!say_index_buffers) {
    say_index_buffers = say_array_create(sizeof(say_global_ibo),
                                         say_global_ibo_free,
                                         NULL);
  }

  size_t i = 0;
  for (say_global_ibo *ibo = say_array_get(say_index_buffers, 0);
       ibo;
       say_array_next(say_index_buffers, (void**)&ibo)) {
    size_t loc;
    if ((loc = say_global_ibo_find_in(ibo, size)) != SAY_MAX_SIZE) {
      *buf_id = i;
      return loc;
    }

    i++;
  }

  /* Could not find enough room in previous buffers. Let's create a new one. */
  size_t buf_size = SAY_BUFFER_MIN_SIZE;
  if (size > SAY_BUFFER_MAX_SIZE)
    buf_size = size;
  else {
    while (buf_size < size)
      buf_size *= 2;
  }

  say_global_ibo ibo = say_global_ibo_create(buf_size);
  say_array_push(say_index_buffers, &ibo);

  *buf_id = say_array_get_size(say_index_buffers) - 1;
  return say_global_ibo_find_in(say_global_ibo_at(*buf_id), size);
}

static void say_global_ibo_delete_at(say_global_ibo *ibo, size_t loc,
                                     size_t range_size) {
  if (!ibo)
    return;

  size_t n = 0, size = say_array_get_size(ibo->ranges);
  for (; n < size; n++) {
    say_range *range = say_array_get(ibo->ranges, n);
    if (range->loc == loc && range->size == range_size)
      break;
  }

  if (n == size)
    return; /* Element could not be found */

  say_array_delete(ibo->ranges, n);
}

static void say_global_ibo_reduce_size(say_global_ibo *ibo, size_t loc,
                                       size_t old_size, size_t size) {
  for (say_range *range = say_array_get(ibo->ranges, 0); range;
       say_array_next(ibo->ranges, (void**)&range)) {
    if (range->loc == loc && range->size == old_size) {
      range->size = size;
      return;
    }
  }
}

static say_index_buffer *say_ibo_at(size_t index) {
  if (!say_index_buffers)
    return NULL;
  return say_global_ibo_at(index)->buf;
}

say_index_buffer_slice *say_index_buffer_slice_create(size_t size) {
  say_index_buffer_slice *slice = malloc(sizeof(say_index_buffer_slice));
  slice->loc  = say_global_ibo_find(size, &slice->buf_id);
  slice->size = size;
  return slice;
}

void say_index_buffer_slice_free(say_index_buffer_slice *slice) {
  say_global_ibo_delete_at(say_global_ibo_at(slice->buf_id), slice->loc,
                           slice->size);
  free(slice);
}

void say_index_buffer_slice_recreate(say_index_buffer_slice *slice,
                                     size_t size) {
 if (size > slice->size) {
   say_global_ibo_delete_at(say_global_ibo_at(slice->buf_id), slice->loc,
                            slice->size);
   slice->loc = say_global_ibo_find(size, &slice->buf_id);
  }
 else {
   say_global_ibo_reduce_size(say_global_ibo_at(slice->buf_id),
                              slice->loc, slice->size, size);
 }

  slice->size = size;
}

size_t say_index_buffer_slice_get_loc(say_index_buffer_slice *slice) {
  return slice->loc;
}

size_t say_index_buffer_slice_get_size(say_index_buffer_slice *slice) {
  return slice->size;
}

GLuint *say_index_buffer_slice_get(say_index_buffer_slice *slice, size_t id) {
  return say_index_buffer_get(say_ibo_at(slice->buf_id), slice->loc + id);
}

void say_index_buffer_slice_update(say_index_buffer_slice *slice) {
  say_index_buffer_update_part(say_ibo_at(slice->buf_id), slice->loc,
                               slice->size);
}

void say_index_buffer_slice_bind(say_index_buffer_slice *slice) {
  say_index_buffer_bind(say_ibo_at(slice->buf_id));
}

void say_index_buffer_slice_clean_up() {
  if (say_index_buffers) {
    say_array_free(say_index_buffers);
    say_index_buffers = NULL;
  }
}
