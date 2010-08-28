module Ray
  class Image
    def inspect
      "#<#{self} w=#{w} h=#{h} bpp=#{bpp}>"
    end

    alias :bits_per_pixel :bpp
    alias :w :width
    alias :h :height

    @@images = Hash.new { |h, k| h[k] = new(k) }

    class << self
      # @param [String] name Filename of the image to load
      # @return [Ray::Image] The image loaded from that file.
      def [](name)
        @@images[name.to_str]
      end

      # Selects the images that should stay in the cache.
      # @yield [filename, image] Block returning true if the image should stay
      #                          in the cache.
      def select!(&block)
        @@images.delete_if { |key, val| !(block.call(key, val)) }
      end

      # Selects the images that should be removed from the cache.
      # @yield [filename, image] Block returning true if the image should be
      #                          removed from the cache.
      def reject!(&block)
        @@images.delete_if(&block)
      end

      alias :delete_if :reject!
    end
  end
end
