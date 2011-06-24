module Ray
  class Animation
    # The target of this animation will move on a circular
    # orbit.
    #
    # Registered as #circular_motion.
    class CircularMotion < Animation
      register_for :circular_motion

      # @option opts [Ray::Vector2] :center Center of the orbit
      # @option opts [Float] :radius Radius of the circle followed by the target.
      # @option opts [Float] :angle Required unless :from and :to are set.
      #   Last value for the angle.
      # @option opts [Float] :from First value for the angle.
      # @option opts [Float] :to Last value for the angle.
      # @option opts [Float] :duration Duration in seconds.
      def setup(opts)
        @center = opts[:center].to_vector2
        @radius = opts[:radius]

        if opts[:angle]
          @from_angle = 0
          @to_angle   = (opts[:angle] / 180) * Math::PI
        else
          @from_angle = (opts[:from] / 180) * Math::PI
          @to_angle   = (opts[:to]   / 180) * Math::PI
        end

        self.duration = opts[:duration]
      end

      def update_target
        angle = @from_angle + (progression / 100) * (@from_angle - @to_angle)

        x = @center.x + (Math.cos(angle) * @radius)
        y = @center.y - (Math.sin(angle) * @radius)

        target.pos = [x, y]
      end

      def end_animation
        x = @center.x + (Math.cos(@to_angle) * @radius)
        y = @center.y - (Math.sin(@to_angle) * @radius)

        target.pos = [x, y]
      end

      # @return [CircularMotion] An animation cancelling self.
      def -@
        circular_motion(:from => (@to_angle / Math::PI) * 180,
                        :to => (@from_angle / Math::PI) * 180,
                        :duration => duration, :center => @center,
                        :radius => @radius)
      end
    end
  end
end
