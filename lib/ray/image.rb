module Ray
  class Image
    def inspect
      "#<#{self} w=#{w} h=#{h} bpp=#{bpp}>"
    end

    alias :bits_per_pixel :bpp
    alias :w :width
    alias :h :height
  end
end
