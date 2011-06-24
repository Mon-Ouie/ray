#include "say.h"

#define SAY_MAX_SIZE ((size_t)-1)

/* 4 MB of vertices per buffer */
#define SAY_BUFFER_NORMAL_SIZE ((4 * 1024 * 1024) / sizeof(say_vertex))

typedef struct {
  size_t loc, size;
} say_range;

#define say_make_range(begin, size) ((say_range){begin, size})

typedef struct {
  say_buffer *buf;
  say_array *ranges;
} say_global_buffer;

static say_global_buffer *say_global_buffers = NULL;
static size_t say_buffer_count = 0;

static void say_global_buffer_create(size_t size) {
  say_global_buffers = realloc(say_global_buffers,
                               sizeof(say_global_buffer) *
                               (say_buffer_count + 1));

  say_global_buffers[say_buffer_count].buf = say_buffer_create(SAY_STREAM,
                                                               size);

  say_global_buffers[say_buffer_count].ranges = say_array_create(sizeof(say_range),
                                                                 NULL,
                                                                 NULL);

  say_buffer_count++;
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
  //printf("removing %zu when we have %zu items\n", loc, buf->range_count);

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

static size_t say_global_buffer_reserve(size_t size, size_t *ret_id) {
  for (size_t i = 0; i < say_buffer_count; i++) {
    size_t res = say_global_buffer_find(&(say_global_buffers[i]), size);
    if (res != SAY_MAX_SIZE) {
      *ret_id = i;
      return res;
    }
  }

  /* Existing buffers can't store this object, save it somewhere else */
  say_global_buffer_create(size > SAY_BUFFER_NORMAL_SIZE ? size :
                           SAY_BUFFER_NORMAL_SIZE);
  *ret_id = say_buffer_count - 1;
  return say_global_buffer_find(&(say_global_buffers[say_buffer_count - 1]), size);
}

static say_global_buffer *say_global_buffer_at(size_t id) {
  return &(say_global_buffers[id]);
}

say_buffer_space *say_buffer_space_create(size_t size) {
  say_buffer_space *space = malloc(sizeof(say_buffer_space));
  space->loc  = say_global_buffer_reserve(size, &space->buf_id);
  space->size = size;
  return space;
}

void say_buffer_space_free(say_buffer_space *space) {
  say_global_buffer_delete_at(say_global_buffer_at(space->buf_id),
                              space->loc);
  free(space);
}

void say_buffer_space_use(say_buffer_space *space) {
  say_buffer_bind(say_global_buffer_at(space->buf_id)->buf);
}

void say_buffer_space_recreate(say_buffer_space *space, size_t size) {
  if (size > space->size) {
    say_global_buffer_delete_at(say_global_buffer_at(space->buf_id),
                                space->loc);
    space->loc = say_global_buffer_reserve(size, &space->buf_id);
  }
  else {
    say_global_buffer_reduce_size(say_global_buffer_at(space->buf_id),
                                  space->loc, size);
  }

  space->size = size;
}

size_t say_buffer_space_get_loc(say_buffer_space *space) {
  return space->loc;
}

size_t say_buffer_space_get_size(say_buffer_space *space) {
  return space->size;
}

say_vertex *say_buffer_space_get_vertex(say_buffer_space *space, size_t id) {
  return say_buffer_get_vertex(say_global_buffer_at(space->buf_id)->buf,
                               space->loc + id);
}

void say_buffer_space_update(say_buffer_space *space) {
  say_buffer_update_part(say_global_buffer_at(space->buf_id)->buf,
                         space->loc, space->size);
}

void say_buffer_space_bind(say_buffer_space *space) {
  say_buffer_bind(say_global_buffer_at(space->buf_id)->buf);
}
