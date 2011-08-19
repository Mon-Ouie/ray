module Ray
  class Vertex
    # Important settings
    @vertex_type_id = 0

    include Ray::PP

    def x; pos.x; end
    def y; pos.y; end

    def x=(val); self.pos = [val, y]; end
    def y=(val); self.pos = [x, val]; end

    def tex_x; tex.x; end
    def tex_y; tex.y; end

    def tex_x=(val); self.tex = [val, tex_y]; end
    def tex_y=(val); self.tex = [tex_x, val]; end

    def pretty_print(q)
      pretty_print_attributes q, ["pos", "col", "tex"]
    end

    alias color  col
    alias color= col=

    alias position  pos
    alias position= pos=
  end
end
