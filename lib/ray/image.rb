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

    # @yield [pixel]
    # @yieldparam [Ray::Color] pixel Color of a point
    def each
      (0...w).each do |x|
        (0...h).each do |y|
          yield self[x, y]
        end
      end

      self
    end

    # Same as each, but also yields the position of each point.
    # @yield [x, y, pixel]
    def each_with_pos
      (0...w).each do |x|
        (0...h).each do |y|
          yield x, y, self[x, y]
        end
      end

      self
    end

    # @yield [pixel] Block returning the new color of this pixel.
    def map!
      lock do
        (0...w).each do |x|
          (0...h).each do |y|
            self[x, y] = yield self[x, y]
          end
        end
      end

      self
    end

    # @yield [x, y, pixel] Block returning the new color of this pixel
    def map_with_pos!
      lock do
        (0...w).each do |x|
          (0...h).each do |y|
            self[x, y] = yield x, y, self[x, y]
          end
        end
      end

      self
    end

    # @return [Ray::Image] New image created according to a block.
    def map(&block)
      dup.map!(&block)
    end

    # @return [Ray::Image] New image created according to a block.
    def map_with_pos(&block)
      dup.map_with_pos!(&block)
    end

    include Enumerable
  end
end
