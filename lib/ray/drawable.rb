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

    # Pretty prints attributes
    #
    # @param [PrettyPrint] q Pretty printer
    # @param [Array<String>] other_attributes Other attributes to be printed
    #
    # @yield Yields to insert custom attributes
    def pretty_print_attributes(q, other_attributes = [])
      id = "%x" % (__id__ * 2)
      id.sub!(/\Af(?=[[:xdigit:]]{2}+\z)/, '') if id.sub!(/\A\.\./, '')

      klass = self.class.pretty_inspect.chomp

      # Note: This doesn't use %w[...] arrays because YARD can't parse them
      attributes = [
                    "origin", "pos", "z", "scale", "angle",
                    "matrix", "matrix_proc",
                    "shader", "shader_attributes",
                    "vertex_count", "index_count", "changed?", "textured?",
                    "blend_mode"
                   ] + other_attributes

      q.group(2, "\#<#{klass}:0x#{id}", '>') do
        q.seplist(attributes, lambda { q.text ',' }) do |key|
          q.breakable

          q.text key.to_s
          q.text '='

          q.group(2) do
            q.breakable ''
            q.pp send(key)
          end
        end

        yield q if block_given?
      end
    end

    # alias pretty_print pretty_print_attributes

    # @return [Hash, nil] Attributes passed to the shader when the object is
    #   drawn.
    attr_accessor :shader_attributes

    alias zoom  scale
    alias zoom= scale=

    alias position  pos
    alias position= pos=
  end
end
