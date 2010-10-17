module Ray
  module SoundSet
    extend Ray::ResourceSet
    
    class << self
      def missing_pattern(string)
        Ray::Sound[string]
      end

      def select!(&block)
        super(&block)
        Ray::Sound.select!(&block)
      end
    end
  end

  # Creates a new sound set.
  #
  # @param [Regexp] regex Regular expression used to match file
  # @yield [*args] Block returning the sound
  #
  # @yieldparam args Regex captures
  def self.sound_set(regex, &block)
    Ray::SoundSet.add_set(regex, &block)
  end
end

begin
  require 'open-uri'

  Ray.sound_set(/^(http|ftp):\/\/(\S+)$/) do |protocol, address|
    open("#{protocol}://#{address}") { |io| Ray::Sound.new(io) }
  end
rescue LoadError
end
