module Ray
  module FontSet
    extend Ray::ResourceSet
    need_argument_count 1

    class << self
      def missing_pattern(string, size)
        Ray::Font[string, size]
      end

      def select!(&block)
        super(&block)
        Ray::Font.select!(&block)
      end
    end
  end

  # Creates a new image set.
  #
  # @param [Regexp] regex Regular expression used to match file
  # @yield [*args, size] Block returning the image matching the captures
  #
  # @yieldparam args Regex captures
  # @yieldparam size Size of the font
  def self.font_set(regex, &block)
    Ray::FontSet.add_set(regex, &block)
  end
end

begin
  require 'open-uri'

  Ray.font_set(/^(http|ftp):\/\/(\S+)$/) do |protocol, address, size|
    open("#{protocol}://#{address}") { |io, size| Ray::Font.new(io, size) }
  end
rescue LoadError
end
