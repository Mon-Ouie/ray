module Ray
  class Target
    # @return [Ray::Shader] Shader used when drawing on this target
    def shader
      @shader ||= simple_shader # must always remain the same object
    end

    # @param [Ray::View] view A new view
    # @yield a block where the view has been changed
    def with_view(view)
      old_view = self.view
      self.view = view
      yield self
    ensure
      self.view = old_view
    end
  end
end
