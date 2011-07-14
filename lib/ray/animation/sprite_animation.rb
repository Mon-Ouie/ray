module Ray
  class Animation
    # Sprite animations are used to animate a position change in a sprite
    # sheet.
    #
    # It is written so that each step of the animation (including the latest
    # one) has the same duration.
    class SpriteAnimation < Ray::Animation
      register_for :sprite_animation

      # @option opts [Ray::Vector2] :from First sprite position
      # @option opts [Ray::Vector2] :to   Last sprite position
      # @option opts [true, false]  :loop Whether to reset to the first step at
      #   the end of the animation.
      # @option opts [Float] :duration Duration of the animation, in seconds.
      def setup(opts)
        self.duration = opts[:duration]

        @initial_value = opts[:from].to_vector2.dup

        delta = opts[:to].to_vector2 - opts[:from].to_vector2

        @x_steps = delta.x.to_i.abs + 1
        @y_steps = delta.y.to_i.abs + 1

        @negative_x = delta.x < 0
        @negative_y = delta.y < 0

        @loop = opts[:loop]
      end

      def update_target
        progress = progression

        x_prog = ((progress / 100) * @x_steps).floor
        y_prog = ((progress / 100) * @y_steps).floor

        x_prog = @x_steps - 1 if x_prog == @x_steps
        y_prog = @y_steps - 1 if y_prog == @y_steps

        target.sheet_pos = @initial_value + variation(x_prog, y_prog)
      end

      def end_animation
        target.sheet_pos = @initial_value if @loop
      end

      def variation(x, y)
        Ray::Vector2[@negative_x ? -x : x, @negative_y ? -y : y]
      end

      def -@
        last_value = @initial_value + variation(@x_steps - 1, @y_steps - 1)

        sprite_animation(:from     => @initial_value,
                         :to       => last_value,
                         :loop     => @loop,
                         :duration => @duration)
      end

      attr_reader :current_value

      attr_reader :initial_value
      attr_reader :x_steps, :y_steps
      attr_reader :negative_x, :negative_y
    end
  end
end
