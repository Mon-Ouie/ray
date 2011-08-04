module Ray
  class PixelBus
    # @group Copying data

    # Duplicates an image through this bus
    #
    # @param [Ray::Image] image Image to be copied
    # @param [Integer] offset Offset to write the pixels to
    #
    # @return [Ray::Image] Copied image
    def copy(image, offset = 0)
      copy = Ray::Image.new image.size

      pull image, :offset => offset
      push copy,  :offset => offset

      copy
    end

    # @endgroup
  end
end
