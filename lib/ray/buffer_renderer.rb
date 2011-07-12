module Ray
  class BufferRenderer
    alias << push

    def drawables
      @drawables.dup # don't let evil users touch this!
    end
  end
end
