require 'enumerator'
require 'iconv'

require 'ray_ext'

module Ray
  # True for big endian
  BigEndian = ([1].pack("I") == [1].pack("N"))

  # Encoding used by Ray for text_entered events.
  InternalEncoding = BigEndian ? "UTF-32BE" : "UTF-32LE"
end

require 'ray/resource_set'

require 'ray/vector'
require 'ray/rect'
require 'ray/matrix'
require 'ray/view'
require 'ray/color'
require 'ray/shader'

require 'ray/image'
require 'ray/text_helper'
require 'ray/font'

require 'ray/target'
require 'ray/window'
require 'ray/image_target'

require 'ray/event'

require 'ray/drawable'
require 'ray/polygon'
require 'ray/sprite'
require 'ray/text'
require 'ray/turtle'

require 'ray/sound_buffer'
require 'ray/sound'
require 'ray/music'

require 'ray/image_set'
require 'ray/font_set'
require 'ray/sound_buffer_set'

require 'ray/dsl'
require 'ray/helper'

require 'ray/animation'
require 'ray/animation_list'

require 'ray/scene_list'
require 'ray/scene'

require 'ray/game'

