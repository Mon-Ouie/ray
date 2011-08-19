module Ray
  class Drawable
    include Ray::PP

    # @group Transformations

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

    # @endgroup

    def pretty_print(q, other_attributes = [])
      # Note: This doesn't use %w[...] arrays because YARD can't parse them
      attributes = [
                    "origin", "pos", "z", "scale", "angle",
                    "matrix", "matrix_proc",
                    "shader", "shader_attributes",
                    "vertex_count", "index_count", "changed?", "textured?",
                    "blend_mode"
                   ]

      pretty_print_attributes q, attributes + other_attributes
    end

    # @return [Hash, nil] Attributes passed to the shader when the object is
    #   drawn.
    attr_accessor :shader_attributes

    alias zoom  scale
    alias zoom= scale=

    alias position  pos
    alias position= pos=
  end
end
