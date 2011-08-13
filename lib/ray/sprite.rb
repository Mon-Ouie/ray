module Ray
  class Sprite < Drawable
    # Creates a sprite.
    #
    # @param [String, Ray::Image] img The image this object will use
    # @option opts [Ray::Vecor2, #to_Vector2] :at ((0,0)) Position of the sprite
    # @option opts [Ray::Rect, Array<Integer>] :rect Rect of the image which will
    #   be drawn.
    # @option opts [Float] :angle (0) The angle which will be used to draw the image
    #   in degrees. Defaults to 0.
    # @option opts [Ray::Vector2] :zoom (1, 1) The zoom level which will be used to draw the image.
    # @option opts [Ray::Vector2] :scale Alias for :zoom
    # @option opts [Ray::Color] :color (Ray::Color.white) Color used, multiplying each pixel of the
    #   sprite.
    # @option opts [true, false] :flip_x (false) Set to true to flip the sprite
    #   horizontally.
    # @option opts [true, false] :flip_y (false) Set to true to flip the sprite
    #   vertically.
    # @option opts [Ray::Vector2] :origin ((0, 0)) The origin of transformations
    # @option opts :shader [Ray::Shader] (nil) Shader
    def initialize(img = nil, opts = {})
      self.image = img.is_a?(String) ? Ray::ImageSet[img] : img

      opts = {
        :at     => Ray::Vector2[0, 0],
        :angle  => 0,
        :zoom   => Ray::Vector2[1, 1],
        :color  => Ray::Color.white,
        :origin => Ray::Vector2[0, 0]
      }.merge(opts)

      self.pos      = opts[:at]
      self.sub_rect = opts[:rect] if opts[:rect]
      self.angle    = opts[:angle]
      self.scale    = opts[:scale] || opts[:zoom]
      self.color    = opts[:color]
      self.flip_x   = opts[:flip_x]
      self.flip_y   = opts[:flip_y]
      self.origin   = opts[:origin]
      self.shader   = opts[:shader]

      # @uses_sprite_sheet = false
      # @sprite_sheet_size = Ray::Vector2[1, 1]
      # @sprite_sheet_pos  = Ray::Vector2[0, 0]
    end

    # @return [Ray::Rect] The rect where this sprite will be drawn, taking
    #   position and scale in account.
    def rect
      pos      = self.pos
      sub_rect = self.sub_rect
      origin   = self.origin
      scale    = self.scale

      top_left = (-origin * scale) + pos

      Ray::Rect.new(top_left.x,
                    top_left.y,
                    sub_rect.w * scale.w,
                    sub_rect.h * scale.h)
    end

    # @param [Ray::Rect, #to_rect] An object with which the receiver may collide
    def collide?(obj)
      rect.collide?(obj.to_rect)
    end

    # @param [Ray::Rect, #to_rect] (See #collide?)
    def inside?(obj)
      rect.inside?(obj.to_rect)
    end

    # @param [Ray::Rect, #to_rect] (See #collide?)
    def outside?(obj)
      rect.outside?(obj.to_rect)
    end

    def to_rect
      rect
    end
  end
end
