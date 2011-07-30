module Ray
  class Rect
    class << self
      alias :[] :new
    end

    def inspect
      "(#{pos}, #{size})"
    end

    alias :to_s :inspect

    # @return [true, false] True if the receiver is inside the rect.
    #                       (false if they just collide)
    def inside?(rect)
      rect = rect.to_rect
      (x >= rect.x) && (y >= rect.y) &&
        (x + w) <= (rect.x + rect.w) &&
        (y + h) <= (rect.y + rect.h)
    end

    # @return [true, false] True if the receiver is outside the rect.
    def outside?(rect)
      !collide?(rect)
    end

    # @return [true, false] True if the receiver collides with the rect.
    def collide?(rect)
      rect = rect.to_rect

      rect.x < x + width &&
        x < rect.x + rect.width &&
        rect.y < y + height &&
        y < rect.y + rect.height
    end

    # @return [true, false] True if the receiver contians this point
    def contain?(p)
      p = p.to_vector2
      (p.x >= x) && (p.y >= y) &&
        (p.x < x + w) && (p.y < y + h)
    end

    # @return [Ray::Vector2] top left corner
    def top_left
      Ray::Vector2[x, y]
    end

    # @return [Ray::Vector2] top right corner
    def top_right
      Ray::Vector2[x + w, y]
    end

    # @return [Ray::Vector2] bottom left corner
    def bottom_left
      Ray::Vector2[x, y + h]
    end

    # @return [Ray::Vector2] bottom right corner
    def bottom_right
      Ray::Vector2[x + w, y + h]
    end

    # @return [Ray::Vector2]
    def center
      Ray::Vector2[x + w / 2, y + h / 2]
    end

    # @return [true, false] True if the two rects are equal
    def ==(rect)
      return false unless rect.is_a? Rect
      x == rect.x && y == rect.y && w == rect.w && h == rect.h
    end

    def eql?(obj)
      self.class == obj.class && self == obj
    end

    def hash
      [x, y, w, h].hash
    end

    def to_rect
      self
    end

    # @return [Ray::Vector2] The position of the rect
    def pos
      Vector2[x, y]
    end

    # @return [Ray::Vector2] The size of the rect
    def size
      Vector2[w, h]
    end

    alias :w :width
    alias :h :height

    alias :w= :width=
    alias :h= :height=
  end
end

class Array
  # @return [Ray::Rect] Converts an array to a rect.
  def to_rect
    Ray::Rect.new(*self)
  end
end

class Hash
  # @return [Ray::Rect] Converts a hash into a rect.
  def to_rect
    Ray::Rect.new(self)
  end
end
