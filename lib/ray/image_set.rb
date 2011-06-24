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
  # @yield [*args] Block returning the image
  # @yieldparam args Regex captures
  def image_set(regex, &block)
    Ray::ImageSet.add_set(regex, &block)
  end

  module_function :image_set
end

require 'open-uri'
Ray.image_set(/^(http|ftp):\/\/(\S+)$/) do |protocol, address|
  open("#{protocol}://#{address}") { |io| Ray::Image.new(io) }
end
