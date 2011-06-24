module Ray
  # Implementation of turtle graphics.
  #
  # A turtle keeps the following caracteristics:
  # - An image which it is manipulating.
  # - Its position (x, y), relative to the image. Set to (0, 0) by default.
  # - An angle, in radians or in degrees (using degrees by default).
  #   Set to 0 by default, i.e. heading East.
  # - Whether the turtle should draw when moved (defaults to true)
  # - The color in which lines will be drawn. Defaults to white.
  class Turtle
    include Math

    # @param [Ray::Target, Ray::image] target the object on which the turtle
    #   should draw. If target is an image, an ImageTarget will be created.
    def initialize(target)
      @target  = target.is_a?(Image) ? Ray::ImageTarget.new(target) : target
      @drawing = true

      @pos = Ray::Vector2[0, 0]
      @angle = 0 # in rad

      @angle_unit = :deg

      @pen_width = 1

      @color = Ray::Color.white
    end

    # Stops drawing
    def pen_up
      @drawing = false
    end

    # Starts drawing
    def pen_down
      @drawing = true
    end

    # @return Whether the turtle is drawing
    def drawing?
      @drawing
    end

    alias pen_down? drawing?

    # @return The opposite of #pen_down?
    def pen_up?
      !@drawing
    end

    # Moves the turtle in the direction it's facing.
    # Draws a line if the pen is down.
    #
    # @param [Float] dist Length of the line
    # @see #backward
    def forward(dist)
      x = pos.x + dist * cos(@angle)
      y = pos.y - dist * sin(@angle) # The y-axis is reversed

      go_to [x, y]
    end

    # Same as forward(-dist)
    # @see #forward
    def backward(dist)
      forward(-dist)
    end

    # Turns the turtle counter-clockwise.
    # @param [Float] angle The angle, in degrees if the angle unit
    #                      is set to degrees. In radians otherwise.
    # @see #right
    def left(angle)
      self.angle += angle
    end

    # Turns the turtle clockwise.
    # @param [Float] angle The angle, in degrees if the angle unit
    #                      is set to degrees. In radians otherwise.
    # @see #left
    def right(angle)
      self.angle -= angle
    end

    # Makes the turtle move to (x, y). Draws if the pen is down.
    def go_to(point)
      if pen_down?
        @target.draw Ray::Polygon.line(pos, point, @pen_width, @color)
      end

      self.pos = point
    end

    # Makes the turtle go to the center. Draws if the pen is down.
    def center
      go_to @target.clip.center
    end

    # Resets the turtle's position, angle, and color but not
    # the content of the image. Also sets the pen down.
    # @see #clear
    def reset
      self.angle     = 0
      self.pos       = [0, 0]
      self.color     = Ray::Color.white
      self.pen_width = 1

      pen_down
    end

    # Clears the content of the image and call reset.
    # @see #reset
    def clear
      @target.clear Ray::Color.none
      reset
    end

    # Returns the distance between the turtle and a point.
    # @see #dist_square
    def dist(point)
      sqrt dist_square(point)
    end

    # Returns the square of the distance between the turtle and a point.
    # Since this doesn't require to call sqrt, it is faster to use this
    # when possible.
    #
    # @see #dist
    def dist_square(point)
      (pos - point.to_vector2).length
    end

    # @return [Ray::Target] The target the turtle is drawing on
    attr_reader :target

    def x
      pos.x
    end

    def y
      pos.y
    end

    def x=(val)
      self.pos = [val, y]
    end

    def y=(val)
      self.pos = [x, val]
    end

    # Sets the position of the turtle.
    # @example
    #   turtle.pos = [10, 20]
    #   turtle.pos = Ray::Vector2[10, 20]
    def pos=(point)
      @pos = point.to_vector2
    end

    attr_reader :pos

    # The unit used for angle. Either :deg (degrees) or :rad
    # (radians)
    attr_reader :angle_unit

    # Sets the angle unit.
    def angle_unit=(unit)
      unless unit == :deg || unit == :rad
        raise ArgumentError, "Unknown angle unit '#{unit}'"
      end

      @angle_unit = unit
    end

    # @return [Float] The angle, either in radians or in degrees,
    #                 depending on angle_unit.
    def angle
      @angle_unit == :deg ? rad_to_deg(@angle) : @angle
    end

    # Sets the angle.
    # @param [Float] val The angle, either in radians or in degrees,
    #                 depending on angle_unit.
    def angle=(val)
      @angle = @angle_unit == :deg ? deg_to_rad(val) : val
    end

    # The color the turtle uses for drawing.
    attr_accessor :color
    alias pen_color color=

    attr_accessor :pen_width

    private
    def deg_to_rad(deg)
      (PI * deg) / 180
    end

    def rad_to_deg(rad)
      (180 * rad) / PI
    end
  end

  class Image
    # (see Ray::Target#turtle)
    def turtle(&block)
      Ray::ImageTarget.new(self).turtle(&block)
    end
  end

  class Target
    # Creates a turtle operating on the receiver.
    # Instance evaluates the block it is given if any.
    # @return [Ray::Turtle] The turtle it created.
    def turtle(&block)
      turtle = Ray::Turtle.new(self)
      if block
        turtle.instance_eval(&block)
        update
      end

      turtle
    end
  end
end
