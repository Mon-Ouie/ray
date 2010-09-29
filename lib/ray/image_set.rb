module Ray
  module ImageSet
    extend Ray::ResourceSet

    class << self
      def missing_pattern(string)
        Ray::Image[string]
      end

      def select!(&block)
        super(&block)
        Ray::Image.select!(&block)
      end
    end
  end

  # Creates a new image set.
  #
  # @param [Regexp] regex Regular expression used to match file
  # @yield [*args] Block returning the image matching the captures
  # @yieldparam args Regex captures
  def self.image_set(regex, &block)
    Ray::ImageSet.add_set(regex, &block)
  end
end

begin
  require 'open-uri'

  Ray.image_set(/^(http|ftp):\/\/(\S+)$/) do |protocol, address|
    open("#{protocol}://#{address}") { |io| Ray::Image.new(io) }
  end
rescue LoadError
  # that image set is not needed
end
