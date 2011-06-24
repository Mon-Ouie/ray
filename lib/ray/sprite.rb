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

    # # Sets the size of the sprite sheet. For instance,
    # #   sprite.sheet_size = [3, 4]
    # # would mean there are 4 rows and 3 columns in the sprite (and each cell
    # # has the same size).
    # def sheet_size=(ary)
    #   @uses_sprite_sheet = true
    #   @sprite_sheet_size = ary.to_vector2

    #   self.sheet_pos = [0, 0]
    # end

    # # Sets which cell of the sprite sheet should be displayed.
    # #   sprite.sheet_pos = [0, 1] # Uses the first cell of the second line.
    # #
    # # pos.x and pos.y are rounded to floor.
    # # Passing a too high value will make the sprite use the previous cells.
    # #    sprite.sheet_size = [4, 4]
    # #    sprite.sheet_size = [5, 5]
    # #    sprite.sheet_size == [1, 1] # => true
    # def sheet_pos=(pos)
    #   pos = pos.to_vector2.dup
    #   pos.x = pos.x.floor % @sprite_sheet_size.w
    #   pos.y = pos.y.floor % @sprite_sheet_size.h

    #   self.sub_rect = Ray::Rect.new(pos.x * sprite_width,
    #                                 pos.y * sprite_height,
    #                                 sprite_width, sprite_height)

    #   @sheet_pos = pos
    # end

    # # @return [Ray::Vector2] the position of the cell which is being used.
    # attr_reader :sheet_pos

    # # @return [Integer, nil] The width of each cell in the sprite sheet
    # def sprite_width
    #   if uses_sprite_sheet?
    #     image.w / @sprite_sheet_size.w
    #   end
    # end

    # # @return [Integer, nil] The height of each cell in the sprite sheet
    # def sprite_height
    #   if uses_sprite_sheet?
    #     image.h / @sprite_sheet_size.h
    #   end
    # end

    # # Disables the sprite sheet
    # def disable_sprite_sheet
    #   self.sub_rect = Ray::Rect.new(0, 0, image.w, image.h) if image

    #   @sprite_sheet_size = nil
    #   @uses_sprite_sheet = false
    # end

    # def uses_sprite_sheet?
    #   @uses_sprite_sheet
    # end

    # @return [Ray::Rect] The rect where this sprite will be drawn, taking
    #   position and scale in account.
    def rect
      pos      = self.pos
      sub_rect = self.sub_rect
      scade    = self.scale

      Ray::Rect.new(pos.x, pos.y, sub_rect.w * scale.w, sub_rect.h * scale.h)
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
