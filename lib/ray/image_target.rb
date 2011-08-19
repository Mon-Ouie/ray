module Ray
  class ImageTarget < Target
    # @yield [target] Yields itself if a block is given
    # @yieldparam [Ray::ImageTarget] target The new target
    #
    # @param [Ray::Image, nil] image The image to use
    def initialize(image = nil)
      self.image = image if image

      if block_given?
        yield self
      end
    end

    def pretty_print(q)
      super q, ["image"]
    end
  end
end
