module Ray
  module SoundBufferSet
    extend Ray::ResourceSet

    class << self
      def missing_pattern(string)
        Ray::SoundBuffer[string]
      end

      def select!(&block)
        super(&block)
        Ray::SoundBuffer.select!(&block)
      end
    end
  end

  # Creates a new sound buffer set.
  #
  # @param [Regexp] regex Regular expression used to match file
  # @yield [*args] Block returning the sound buffer
  # @yieldparam args Regex captures
  def sound_buffer_set(regex, &block)
    Ray::SoundBufferSet.add_set(regex, &block)
  end

  module_function :sound_buffer_set
end

require 'open-uri'
Ray.sound_buffer_set(/^(http|ftp):\/\/(\S+)$/) do |protocol, address|
  open("#{protocol}://#{address}") { |io| Ray::SoundBuffer.new(io) }
end
