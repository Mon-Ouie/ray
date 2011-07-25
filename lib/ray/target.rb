module Ray
  class Target
    # Shader used when drawing on this target
    #
    # Notice this method will always return the same object, and that modifying
    # this object will modify the shader used internally be the target.
    #
    # @return [Ray::Shader] Shader used when drawing on this target
    def shader
      @shader ||= simple_shader # must always remain the same object
    end

    # @group Manipulating views

    # Changes the view temporarily
    #
    # @param [Ray::View] view A new view
    # @yield a block where the view has been changed
    #
    # @example
    #   old_view = target.view
    #   target.with_view new_view do
    #     target.view == new_view # => true
    #   end
    #
    #   target.view == old_view # => true
    def with_view(view)
      old_view = self.view
      self.view = view
      yield self
    ensure
      self.view = old_view
    end

    # @endgroup
  end
end
