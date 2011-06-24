module Ray
  module FontSet
    extend Ray::ResourceSet

    class << self
      def missing_pattern(string)
        Ray::Font[string]
      end

      def select!(&block)
        super(&block)
        Ray::Font.select!(&block)
      end
    end
  end

  # Creates a new font set.
  #
  # @param [Regexp] regex Regular expression used to match file
  # @yield [*args] Block returning the font
  #
  # @yieldparam args Regex captures
  def font_set(regex, &block)
    Ray::FontSet.add_set(regex, &block)
  end

  module_function :font_set
end

require 'open-uri'
Ray.font_set(/^(http|ftp):\/\/(\S+)$/) do |protocol, address|
  open("#{protocol}://#{address}") { |io| Ray::Font.new(io) }
end
