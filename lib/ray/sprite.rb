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

      @from_rect = opts[:rect].to_rect

      rect = opts[:at].to_rect
      @x, @y = rect.x, rect.y

      self.angle = opts[:angle]
      self.zoom  = opts[:zoom]

      @image = image.to_image
    end
    
    # Sets the size of the sprite sheet. For instance,
    #   sprite.sheet_size = [3, 4]
    # would mean there are 4 rows and 3 columns in the sprite (and each cell
    # has the same size).
    def sheet_size=(ary)
      w, h = ary
      
      @uses_sprite_sheet = true
      
      @sprite_sheet_w = w
      @sprite_sheet_h = h
      
      self.sheet_pos = [0, 0]
    end
    
    # Sets which cell of the sprite sheet should be displayed.
    #   sprite.sheet_pos = [0, 1] # Uses the first cell of the second line.
    def sheet_pos=(ary)
      x, y = ary
      
      self.from_rect = Ray::Rect.new(x * sprite_width, y * sprite_height,
                                     sprite_width, sprite_height)
      
      @sheet_pos_x = x
      @sheet_pos_y = y
    end
    
    # Returns the position of the cell which is being used.
    def sheet_pos
      [@sheet_pos_x, @sheet_pos_y]
    end
    
    # @return [Integer, nil] The width of each cell in the sprite sheet
    def sprite_width
      if uses_sprite_sheet?
        @image.w / @sprite_sheet_w
      end
    end
    
    # @return [Integer, nil] The height of each cell in the sprite sheet
    def sprite_height
      if uses_sprite_sheet?
        @image.h / @sprite_sheet_h
      end
    end
    
    # Disables the sprite sheet
    def disable_sprite_sheet
      self.from_rect = Ray::Rect.new(0, 0, @image.w, @image.h)
      
      @sprite_sheet_w = nil
      @sprite_sheet_h = nil
      
      @uses_sprite_sheet = false
    end
    
    def uses_sprite_sheet?
      @uses_sprite_sheet
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
      rect.collide?(obj.to_rect)
    end

    # @param [Ray::Rect, #rect] (See #collide?)
    # @return [true, false]
    def inside?(obj)
      rect.inside?(obj.to_rect)
    end

    # @param [Ray::Rect, #rect] (See #collide?)
    # @return [true, false]
    def outside?(obj)
      rect.outside?(obj.to_rect)
    end

    def to_rect
      rect
    end

    # @return [Ray::Image]
    attr_reader :image

    # @return [Integer] position of the sprite
    attr_accessor :x, :y

    # @return [Ray::Rect] the part of the image which will be drawn. An empty
    #                     rect means the whole image.
    attr_accessor :from_rect
    
    # @return [Ray::Rect] the position of the sprite
    def pos
      [x, y]
    end
    
    # Sets the position of the sprite
    def pos=(ary)
      rect = ary.to_rect
      
      self.x = rect.x
      self.y = rect.y
    end

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
