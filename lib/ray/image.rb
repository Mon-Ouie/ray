module Ray
  class Image
    def inspect
      "#<#{self} w=#{w} h=#{h} bpp=#{bpp}>"
    end

    alias :bits_per_pixel :bpp
    alias :width :w
    alias :height :h
  end
end
