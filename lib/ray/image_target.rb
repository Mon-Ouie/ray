module Ray
  class ImageTarget < Target
    # @param [Ray::Image, nil] image The image to use
    def initialize(image = nil)
      self.image = image if image

      if block_given?
        yield self
      end
    end
  end
end
