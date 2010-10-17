module Ray
  module MusicSet
    extend Ray::ResourceSet
    
    class << self
      def missing_pattern(string)
        Ray::Music[string, size]
      end

      def select!(&block)
        super(&block)
        Ray::Music.select!(&block)
      end
    end
  end

  # Creates a new music set.
  #
  # @param [Regexp] regex Regular expression used to match file
  # @yield [*args] Block returning the music
  #
  # @yieldparam args Regex captures
  def self.music_set(regex, &block)
    Ray::MusicSet.add_set(regex, &block)
  end
end

begin
  require 'open-uri'

  Ray.music_set(/^(http|ftp):\/\/(\S+)$/) do |protocol, address|
    open("#{protocol}://#{address}") { |io| Ray::Music.new(io) }
  end
rescue LoadError
end
