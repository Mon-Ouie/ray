module Ray
  module ImageSet
    @@sets = {}

    class << self
      # @return [Ray::Image] image corresponding to the name.
      def [](name)
        @@sets.each do |regex, hash|
          return hash[name] if name =~ regex
        end

        Ray::Image[name]
      end

      def add_set(regex, &block)
        @@sets[regex] = Hash.new do |hash, key|
          key =~ regex
          hash[key] = block.call(*($~.captures))
        end
      end

      # @see Ray::Image.select!
      def select!(&block)
        @@sets.each do |regex, hash|
          hash.delete_if { |k, v| !(block.call(k, v)) }
        end

        Ray::Image.select!(&block)
      end

      # @see Ray::Image.reject!
      def reject!(&block)
        @@sets.each do |regex, hash|
          hash.delete_if { |k, v| (block.call(k, v)) }
        end

        Ray::Image.reject!(&block)
      end

      alias :delete_if :reject!
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

module Kernel
  # @see Ray.image_set
  def image_set(regex, &block)
    Ray.image_set(regex, &block)
  end

  module_function :image_set
end
