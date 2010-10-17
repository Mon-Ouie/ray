require 'enumerator'

# 'ray_ext' doesn't exist on the PSP.
# We assume that Ray isn't defined in other cases.
require 'ray_ext' unless defined? Ray

require 'ray/resource_set'

require 'ray/color'
require 'ray/rect'
require 'ray/image'
require 'ray/joystick'
require 'ray/font' if Ray.has_font_support?
require 'ray/audio' if Ray.has_audio_support?

require 'ray/image_set'
require 'ray/font_set'
require 'ray/sound_set' if Ray.has_audio_support?
require 'ray/music_set' if Ray.has_audio_support?

require 'ray/dsl'
require 'ray/helper'

require 'ray/sprite'

require 'ray/scene'
require 'ray/game'
