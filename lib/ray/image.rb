module Ray
  class Image
    include Enumerable
    include Ray::PP

    extend Ray::ResourceSet
    add_set(/^(.*)$/) { |filename| new(filename) }

    # @group Iterating over an image

    # Iterates over all the pixels of an image
    #
    # Iteration is done over each pixel from a line from left to right,
    # and each line from top to bottom.
    #
    # @yield [pixel]
    # @yieldparam [Ray::Color] pixel Color of the pixel
    def each
      return Enumerator.new(self, :each) unless block_given?

      (0...h).each do |y|
        (0...w).each do |x|
          yield self[x, y]
        end
      end

      self
    end

    # Yields all the pixels and their positions
    #
    # @yield [pixel, x, y]
    # @yieldparam [Ray::Color] pixel Color of the pixel
    # @yieldparam [Integer] x X position of the pixel
    # @yieldparam [Integer] y Y position of the pixel
    def each_with_pos
      return Enumerator.new(self, :each_with_pos) unless block_given?

      (0...h).each do |y|
        (0...w).each do |x|
          yield self[x, y], x, y
        end
      end

      self
    end

    # Modifies each pixel of the image in-place
    #
    # @yield [pixel]
    # @yieldparam (see #each)
    #
    # @yieldreturn [Ray::Color] New color of the pixel
    def map!
      return Enumerator.new(self, :map!) unless block_given?

      (0...h).each do |y|
        (0...w).each do |x|
          self[x, y] = yield self[x, y]
        end
      end

      self
    end

    # Modifies each pixel of the image in-place, passing their position to the
    # block
    #
    # @yield [pixel, x, y]
    # @yieldparam (see #each_with_pos)
    #
    # @yieldreturn (see #mapV)
    def map_with_pos!
      return Enumerator.new(self, :map_with_pos!) unless block_given?

      (0...h).each do |y|
        (0...w).each do |x|
          self[x, y] = yield self[x, y], x, y
        end
      end

      self
    end

    # Creates a new image using a block
    #
    # @yield [pixel]
    # @yieldparam (see #map!)
    # @yieldreturn (see #map!)
    #
    # @return [Ray::Image] New image created according to a block
    def map(&block)
      dup.map!(&block)
    end

    # Creates a new image using a block, passing the position of each point
    # to it
    #
    # @yield [pixel, x, y]
    # @yieldparam (see #map_with_pos!)
    #
    # @return (see #map)
    def map_with_pos(&block)
      dup.map_with_pos!(&block)
    end

    # @endgroup

    def inspect
      "#<#{self.class} size=#{size}>"
    end

    def pretty_print(q)
      pretty_print_attributes q, ["size", "smooth?"]
    end

    alias w width
    alias h height
  end
end
