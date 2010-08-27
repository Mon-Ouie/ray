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
      def [](name)
        @@images[name.to_str]
      end

      def select!(&block)
        @@images.delete_if { |key, val| !(block.call(key, val)) }
      end

      def reject!(&block)
        @@images.delete_if(&block)
      end

      alias :delete_if :reject!
    end
  end
end
