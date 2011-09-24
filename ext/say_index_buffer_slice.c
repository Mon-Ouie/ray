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
  mo_list           ranges;
} say_global_ibo;

static mo_array *say_index_buffers = NULL;

static say_global_ibo say_global_ibo_create(size_t size) {
  say_global_ibo ret;
  ret.buf    = say_index_buffer_create(SAY_STREAM, size);
  mo_list_init(&ret.ranges, sizeof(say_range));

  return ret;
}

static void say_global_ibo_free(void *data) {
  say_global_ibo *ibo = (say_global_ibo*)data;

  say_index_buffer_free(ibo->buf);
  mo_list_release(&ibo->ranges);
}

static say_global_ibo *say_global_ibo_at(size_t index) {
  if (!say_index_buffers)
    return NULL;
  return mo_array_at(say_index_buffers, index);
}

static bool say_global_ibo_fit_into(say_global_ibo *ibo, size_t used,
                                    size_t size) {
  size_t buffer_size = say_index_buffer_get_size(ibo->buf);

  if (used + size < buffer_size)
    return true;
  else if (used + size < SAY_BUFFER_MAX_SIZE &&
           buffer_size <= SAY_BUFFER_MAX_SIZE) {
    size_t sought_size = used + size;
    size_t right_size  = buffer_size;

    while (right_size < sought_size) right_size *= 2;

    say_index_buffer_resize(ibo->buf, right_size);
    return true;
  }
  else
    return false;
}

static size_t say_global_ibo_prepend(say_global_ibo *ibo, size_t size) {
  say_range range = say_make_range(0, size);
  mo_list_prepend(&ibo->ranges, ibo->ranges.head, &range);
  return 0;
}

static size_t say_global_ibo_insert(mo_list *list, mo_list_it *it,
                                    size_t size) {
  say_range *range = mo_list_it_data_ptr(it, say_range);
  say_range tmp = say_make_range(range->loc + range->size, size);

  mo_list_insert(list, it, &tmp);
  return tmp.loc;
}

static size_t say_global_ibo_find_in(say_global_ibo *ibo, size_t size) {
  if (!ibo->ranges.head && say_global_ibo_fit_into(ibo, 0, size)) {
    return say_global_ibo_prepend(ibo, size);
  }

  say_range *first = mo_list_it_data_ptr(ibo->ranges.head, say_range);

  /* There's room at the begin of the buffer */
  if (first->loc >= size) {
    return say_global_ibo_prepend(ibo, size);
  }

  mo_list_it *it = ibo->ranges.head;
  for (; it->next; it = it->next) {
    say_range *current = mo_list_it_data_ptr(it, say_range);
    say_range *next    = mo_list_it_data_ptr(it->next, say_range);

    size_t begin = current->loc + current->size;
    size_t end   = next->loc;

    /* There's enough room between those two elements */
    if (end - begin >= size)
      return say_global_ibo_insert(&ibo->ranges, it, size);
  }

  say_range *last = mo_list_it_data_ptr(it, say_range);

  /* There's enough room at the end of the buffer */
  if (say_global_ibo_fit_into(ibo, last->loc + last->size, size))
    return say_global_ibo_insert(&ibo->ranges, it, size);
  else
    return SAY_MAX_SIZE;
}

static size_t say_global_ibo_find(size_t size, size_t *buf_id) {
  if (!say_index_buffers) {
    say_index_buffers = mo_array_create(sizeof(say_global_ibo));
    say_index_buffers->release = say_global_ibo_free;
  }

  for (size_t i = 0; i < say_index_buffers->size; i++) {
    say_global_ibo *ibo = mo_array_at(say_index_buffers, 0);

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
  mo_array_push(say_index_buffers, &ibo);

  *buf_id = say_index_buffers->size - 1;
  return say_global_ibo_find_in(say_global_ibo_at(*buf_id), size);
}

static void say_global_ibo_delete_at(say_global_ibo *ibo, size_t loc,
                                     size_t range_size) {
  if (!ibo)
    return;

  for (mo_list_it *it = ibo->ranges.head; it; it = it->next) {
    say_range *range = mo_list_it_data_ptr(it, say_range);
    if (range->loc == loc && range->size == range_size) {
      mo_list_delete(&ibo->ranges, it);
      return;
    }
  }
}

static void say_global_ibo_reduce_size(say_global_ibo *ibo, size_t loc,
                                       size_t old_size, size_t size) {
  for (mo_list_it *it = ibo->ranges.head; it; it = it->next) {
    say_range *range = mo_list_it_data_ptr(it, say_range);
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
    mo_array_free(say_index_buffers);
    say_index_buffers = NULL;
  }
}
