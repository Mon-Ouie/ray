module Ray
  class Sprite
    # Creates a sprite.
    #
    # @param [String, Ray::Image] The image this object will wrap;
    # @option opts [Ray::Rect, Array<Integer>] :at Position of the sprite
    #                                              (defaults to 0,0)
    # @option opts [Ray::Rect, Array<Integer>] :rect Rect of the image which will
    #                                                be drawn (defaults to the
    #                                                whole image)
    # @option opts [Float] :angle The angle which will be used to draw the image
    #                              in degrees. Defaults to 0.
    # @option opts [Float] :zoom The zoom level which will be used to draw the image.
    #                              Defaults to 1.
    def initialize(image, opts = {})
      opts = {
        :rect  => Ray::Rect.new(0, 0, 0, 0),
        :at    => [0, 0],
        :angle => 0.0,
        :zoom  => 1
      }.merge(opts)

      @from_rect = Ray.convert(opts[:rect], :rect)

      rect = Ray.convert(opts[:at], :rect)
      @x, @y = rect.x, rect.y

      self.angle = opts[:angle]
      self.zoom  = opts[:zoom]

      @image = Ray.convert(image, :image)
    end

    # Draws the sprite on an image.
    # @param [Ray::Image] screen The image on which the sprite will be drawn
    def draw_on(screen)
      @image.blit(:rect => @from_rect, :on => screen, :at => [@x, @y],
                  :angle => @angle, :zoom => @zoom)
    end

    # Draws the sprite on the screen or on another image
    # @option opts [Ray::Image] :on The image we should draw on. Defaults to
    #                               Ray.screen.
    def draw(opts = {})
      draw_on(opts[:on] || Ray.screen)
    end

    # @return [true, false] True if the sprite is located at (x, y)
    def is_at?(x, y)
      @x == x && @y == y
    end

    # @return [Ray::Rect] The rect where this sprite will be drawn.
    def rect
      Ray::Rect.new(@x, @y, @from_rect.w == 0 ? @image.w : @from_rect.w,
                    @from_rect.h == 0 ? @image.h : @from_rect.h)
    end

    # @param [Ray::Rect, #rect] An object with which the receiver may collide
    # @return [true, false]
    def collide?(obj)
      rect.colide?(obj.is_a?(Ray::Rect) || obj.is_a?(Array) ? obj : obj.rect)
    end

    # @param [Ray::Rect, #rect] (See #collide?)
    # @return [true, false]
    def inside?(obj)
      rect.inside?(obj.is_a?(Ray::Rect) || obj.is_a?(Array) ? obj : obj.rect)
    end

    # @param [Ray::Rect, #rect] (See #collide?)
    # @return [true, false]
    def outside?(obj)
      rect.outside?(obj.is_a?(Ray::Rect) || obj.is_a?(Array) ? obj : obj.rect)
    end

    # @return [Ray::Image]
    attr_reader :image

    # @return [Integer] position of the sprite
    attr_accessor :x, :y

    # @return [Ray::Rect] the part of the image which will be drawn. An empty
    #                     rect means the whole image.
    attr_accessor :from_rect

    # @return [Float] The angle used when the image is drawn.
    def angle
      @angle ? @angle : 0
    end

    # Sets the angle.
    def angle=(val)
      @angle = (val % 360).zero? ? nil : val
    end

    # @return [Float] the zoom applied to the image when it is drawn.
    def zoom
      @zoom ? @zoom : 1
    end

    # Sets the zoom level.
    def zoom=(val)
      @zoom = val == 1 ? nil : val
    end
  end
end
