module Ray
  class Image
    include Enumerable

    def inspect
      "#<#{self.class} size=#{size}>"
    end

    alias :w :width
    alias :h :height

    extend Ray::ResourceSet
    add_set(/^(.*)$/) { |filename| new(filename) }

    # @yield [pixel]
    # @yieldparam [Ray::Color] pixel Color of a point
    def each
      return Enumerator.new(self, :each) unless block_given?

      (0...h).each do |y|
        (0...w).each do |x|
          yield self[x, y]
        end
      end

      self
    end

    # Same as each, but also yields the position of each point.
    # @yield [pixel, x, y]
    def each_with_pos
      return Enumerator.new(self, :each_with_pos) unless block_given?

      (0...h).each do |y|
        (0...w).each do |x|
          yield self[x, y], x, y
        end
      end

      self
    end

    # @yield [pixel] Block returning the new color of this pixel.
    def map!
      return Enumerator.new(self, :map!) unless block_given?

      (0...h).each do |y|
        (0...w).each do |x|
          self[x, y] = yield self[x, y]
        end
      end

      self
    end

    # @yield [pixel, x, y] Block returning the new color of this pixel
    def map_with_pos!
      return Enumerator.new(self, :map_with_pos!) unless block_given?

      (0...h).each do |y|
        (0...w).each do |x|
          self[x, y] = yield self[x, y], x, y
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
  end
end
