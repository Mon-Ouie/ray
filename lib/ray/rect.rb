module Ray
  class Rect
    def inspect
      "#<#{self.class} {{#{x}, #{y}}, {#{w}, #{h}}}>"
    end

    alias :w :width
    alias :h :height

    alias :w= :width=
    alias :h= :height=
  end
end
