#include "say.h"

static ALCdevice  *say_audio_device  = NULL;
static ALCcontext *say_audio_context = NULL;

void say_audio_context_ensure() {
  if (!say_audio_device) {
    say_audio_device = alcOpenDevice(NULL);
    if (!say_audio_device) {
      say_error_set("could not open audio device");
      return;
    }
  }

  if (!say_audio_context) {
    say_audio_context = alcCreateContext(say_audio_device, NULL);
    if (!say_audio_context) {
      say_error_set("could not create audio context");
      return;
    }

    alcMakeContextCurrent(say_audio_context);
  }
}
