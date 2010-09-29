module Ray
  class Image
    def inspect
      "#<#{self.class} w=#{w} h=#{h}>"
    end

    alias :bits_per_pixel :bpp
    alias :w :width
    alias :h :height

    alias :draw :blit
    alias :update :flip

    include Ray::ResourceSet
    add_set(/^(.*)$/) { |filename| new(filename) }

    # @yield [pixel]
    # @yieldparam [Ray::Color] pixel Color of a point
    def each
      return Enumerator.new(self, :each) unless block_given?

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
      return Enumerator.new(self, :each_with_pos) unless block_given?

      (0...w).each do |x|
        (0...h).each do |y|
          yield x, y, self[x, y]
        end
      end

      self
    end

    # @yield [pixel] Block returning the new color of this pixel.
    def map!
      return Enumerator.new(self, :map!) unless block_given?

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
      return Enumerator.new(self, :map_with_pos!) unless block_given?

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

    def to_image
      self
    end

    include Enumerable
  end
end

class String
  # Converts the string to an image using Ray::ImageSet.[]
  def to_image
    Ray::ImageSet[self]
  end
end
