module Ray
  class Drawable
    def y
      pos.y
    end

    def x
      pos.x
    end

    def y=(val)
      self.pos = [x, val]
    end

    def x=(val)
      self.pos = [val, y]
    end

    def scale_x
      scale.x
    end

    def scale_y
      scale.y
    end

    def scale_x=(val)
      self.scale = [val, scale_y]
    end

    def scale_y=(val)
      self.scale = [scale_x, val]
    end

    alias zoom  scale
    alias zoom= scale=
  end
end
