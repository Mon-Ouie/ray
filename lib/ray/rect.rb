module Ray
  class Rect
    def inspect
      "#<#{self.class} {{#{x}, #{y}}, {#{w}, #{h}}}>"
    end

    # @return [true, false] True if the receiver is inside the rect.
    #                       (false if they just collide)
    def inside?(rect)
      (x >= rect.x) && (y >= rect.y) &&
        (x + w) <= (rect.x + rect.w) &&
        (y + h) <= (rect.y + rect.h)
    end

    # @return [true, false] True if the receiver is outside the rect.
    def outside?(rect)
      !rect.contain?(x, y) &&
        !rect.contain?(x, y + h) &&
        !rect.contain?(x + w, y) &&
        !rect.contain?(x + w, y + h)
    end

    # @return [true, false] True if the receiver collides with the rect.
    def collide?(rect)
      !outside?(rect)
    end

    # @return [true, false] True if the receiver contians this point
    def contain?(p_x, p_y)
      (p_x >= x) && (p_y >= x) &&
        (p_x < x + w) && (p_y < y + h)
    end

    alias :w :width
    alias :h :height

    alias :w= :width=
    alias :h= :height=
  end
end
