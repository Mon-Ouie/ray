#include "say.h"

void say_clean_up() {
  say_audio_context_clean_up();
  say_buffer_slice_clean_up();
  say_index_buffer_slice_clean_up();
  say_error_clean_up();
  say_font_clean_up();

  say_vertex_type_clean_up(); /* NB: Buffers may be using this */

  /* Call last, in case a contex is needed to clean other stuff */
  say_context_clean_up();
}
