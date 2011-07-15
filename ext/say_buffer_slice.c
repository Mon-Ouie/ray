#include "say.h"

#define SAY_MAX_SIZE ((size_t)-1)

/* 4 MB of vertices per buffer */
#define SAY_BUFFER_BYTE_SIZE ((4 * 1024 * 1024))

typedef struct {
  size_t loc, size;
} say_range;

#define say_make_range(begin, size) ((say_range){begin, size})

typedef struct {
  say_buffer *buf;
  say_array *ranges;
} say_global_buffer;

static say_array *say_global_buffers = NULL;

static void say_global_buffer_free(say_global_buffer *buf) {
  say_buffer_free(buf->buf);
  say_array_free(buf->ranges);
}

static say_global_buffer *say_global_buffer_create(say_array *bufs,
                                                   size_t vtype, size_t size) {
  say_global_buffer buffer;

  buffer.buf    = say_buffer_create(vtype, SAY_STREAM, size);
  buffer.ranges = say_array_create(sizeof(say_range), NULL, NULL);

  say_array_push(bufs, &buffer);

  return say_array_get(bufs, say_array_get_size(bufs) - 1);
}


static void say_global_buffer_array_alloc(say_array **ary) {
  *ary = say_array_create(sizeof(say_global_buffer),
                          (say_destructor)say_global_buffer_free,
                          NULL);
}

static void say_global_buffer_array_free(say_array **ary) {
  if (*ary)
    say_array_free(*ary);
}

static size_t say_global_buffer_add_range_before(say_global_buffer *buf,
                                                 size_t before, size_t n) {
  say_range tmp = say_make_range(0, n);
  say_array_insert(buf->ranges, before, &tmp);

  say_range *range = say_array_get(buf->ranges, before);

  if (before != 0) {
    say_range *prev = say_array_get(buf->ranges, before - 1);
    range->loc = prev->loc + prev->size;
  }

  return range->loc;
}

static size_t say_global_buffer_find(say_global_buffer *buf, size_t n) {
  /* Buffer too small for this object */
  if (n > say_buffer_get_size(buf->buf))
    return SAY_MAX_SIZE;

  size_t     ary_size = say_array_get_size(buf->ranges);
  say_range *first    = say_array_get(buf->ranges, 0);

  /* There's room at the begin of the buffer */
  if (ary_size == 0 || first->loc >= n) {
    return say_global_buffer_add_range_before(buf, 0, n);
  }

  say_range *current = first, *next = NULL;

  for (size_t i = 0; i < ary_size - 1; i++) {
    next = say_array_get(buf->ranges, i + 1);

    size_t begin = current->loc + current->size;
    size_t end   = next->loc;

    /* There's enough room between those two elements */
    if (end - begin >= n) {
      return say_global_buffer_add_range_before(buf, i + 1, n);
    }

    current = next;
  }

  say_range *last = say_array_get(buf->ranges, ary_size - 1);

  /* There's enough room at the end of the buffer */
  if ((last->loc + last->size + n) < say_buffer_get_size(buf->buf)) {
    return say_global_buffer_add_range_before(buf, ary_size, n);
  }

  /* Not enough room here */
  return SAY_MAX_SIZE;
}

static void say_global_buffer_delete_at(say_global_buffer *buf, size_t loc) {
  if (!buf)
    return;

  size_t n = 0, size = say_array_get_size(buf->ranges);
  for (; n < size; n++) {
    say_range *range = say_array_get(buf->ranges, n);
    if (range->loc == loc)
      break;
  }

  if (n == size)
    return; /* Element could not be found */

  say_array_delete(buf->ranges, n);
}

static void say_global_buffer_reduce_size(say_global_buffer *buf, size_t loc,
                                          size_t size) {
  for (say_range *range = say_array_get(buf->ranges, 0); range;
       say_array_next(buf->ranges, (void**)&range)) {
    if (range->loc == loc) {
      range->size = size;
      return;
    }
  }
}

static size_t say_global_buffer_reserve(size_t vtype, size_t size, size_t *ret_id) {
  if (!say_global_buffers) {
    say_global_buffers = say_array_create(sizeof(say_array*),
                                          (say_destructor)say_global_buffer_array_free,
                                          (say_creator)say_global_buffer_array_alloc);
  }

  if (say_array_get_size(say_global_buffers) <= vtype) {
    say_array_resize(say_global_buffers, vtype + 1);
  }

  say_array *global_bufs = *(say_array**)say_array_get(say_global_buffers,
                                                       vtype);

  size_t i = 0;
  for (say_global_buffer *buf = say_array_get(global_bufs, 0);
       buf;
       say_array_next(global_bufs, (void**)&buf)) {
    size_t res = say_global_buffer_find(buf, size);

    if (res != SAY_MAX_SIZE) {
      *ret_id = i;
      return res;
    }

    i++;
  }

  /* Existing buffers can't store this object, save it somewhere else */

  say_vertex_type *type        = say_get_vertex_type(vtype);
  size_t           elem_size   = say_vertex_type_get_size(type);
  size_t           normal_size = SAY_BUFFER_BYTE_SIZE /
    (elem_size == 0 ? 1 : elem_size);
  size_t           buf_size    = normal_size > size ? normal_size : size;

  say_global_buffer *buf = say_global_buffer_create(global_bufs,
                                                    vtype,
                                                    buf_size);
  *ret_id = say_array_get_size(global_bufs) - 1;
  return say_global_buffer_find(buf, size);
}

static say_global_buffer *say_global_buffer_at(size_t vtype, size_t id) {
  if (!say_global_buffers)
    return NULL;

  return say_array_get(*(say_array**)say_array_get(say_global_buffers, vtype),
                       id);
}

say_buffer_slice *say_buffer_slice_create(size_t vtype, size_t size) {
  say_buffer_slice *slice = malloc(sizeof(say_buffer_slice));
  slice->loc   = say_global_buffer_reserve(vtype, size, &slice->buf_id);
  slice->vtype = vtype;
  slice->size  = size;
  return slice;
}

void say_buffer_slice_free(say_buffer_slice *slice) {
  say_global_buffer_delete_at(say_global_buffer_at(slice->vtype, slice->buf_id),
                              slice->loc);
  free(slice);
}

void say_buffer_slice_use(say_buffer_slice *slice) {
  say_buffer_bind(say_global_buffer_at(slice->vtype, slice->buf_id)->buf);
}

void say_buffer_slice_recreate(say_buffer_slice *slice, size_t size) {
  if (size > slice->size) {
    say_global_buffer_delete_at(say_global_buffer_at(slice->vtype,
                                                     slice->buf_id),
                                slice->loc);

    slice->loc = say_global_buffer_reserve(slice->vtype, size, &slice->buf_id);
  }
  else if (size == 0) {
    say_global_buffer_delete_at(say_global_buffer_at(slice->vtype,
                                                     slice->buf_id),
                                slice->loc);
    slice->loc = 0;
  }
  else {
    say_global_buffer_reduce_size(say_global_buffer_at(slice->vtype,
                                                       slice->buf_id),
                                  slice->loc, size);
  }

  slice->size = size;
}

size_t say_buffer_slice_get_loc(say_buffer_slice *slice) {
  return slice->loc;
}

size_t say_buffer_slice_get_size(say_buffer_slice *slice) {
  return slice->size;
}

void *say_buffer_slice_get_vertex(say_buffer_slice *slice, size_t id) {
  return say_buffer_get_vertex(say_global_buffer_at(slice->vtype,
                                                    slice->buf_id)->buf,
                               slice->loc + id);
}

void say_buffer_slice_update(say_buffer_slice *slice) {
  say_buffer_update_part(say_global_buffer_at(slice->vtype, slice->buf_id)->buf,
                         slice->loc, slice->size);
}

void say_buffer_slice_bind(say_buffer_slice *slice) {
  say_buffer_bind(say_global_buffer_at(slice->vtype, slice->buf_id)->buf);
}

void say_buffer_slice_clean_up() {
  if (say_global_buffers) {
    say_array_free(say_global_buffers);
    say_global_buffers = NULL;
  }
}
