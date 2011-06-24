module Ray
  class Animation
    # Animates the transition from a vector to another one.
    # Registered as #vector_variation.
    class VectorVariation < Animation
      register_for :vector_variation

      # @option opts [Ray::Vector2, Ray::Vector3] :of [r, g, b, a] Variation for
      #   the vector. Required unless :from and :to are set.
      # @option opts [Ray::Vector2, Ray::Vector3] :from Initial value of the vector.
      # @option opts [Ray::Vector2, Ray::Vector3] :to Last value of the vector.
      # @option opts [Symbol] :attribute Attribute of the target to change.
      # @option opts [Float] :duration Duration in seconds.
      def setup(opts)
        if opts[:of]
          @current_value = @initial_value = nil
          @variation = opts[:of]

          if (@variation.is_a?(Array) && @variation.size == 3) ||
              @variation.is_a?(Ray::Vector3)
            @variation = @variation.to_vector3
          else
            @variation = @variation.to_vector2
          end
        else
          if (opts[:from].is_a?(Array) && opts[:from].size == 2) ||
              opts[:from].is_a?(Ray::Vector2)
            @initial_value = opts[:from].to_vector2
            @variation = opts[:to].to_vector2 - @initial_value
          else
            @initial_value = opts[:from].to_vector3
            @variation = opts[:to].to_vector3 - @initial_value
          end
        end

        @attribute = opts[:attribute]

        self.duration = opts[:duration]
      end

      def setup_target
        unless @initial_value
          @current_value = target.send(@attribute)
        else
          @current_value = @initial_value.dup
        end

        @animations = []
        @animations << float_variation(:of => @variation.x, :attribute => :x,
                                       :duration => duration)
        @animations << float_variation(:of => @variation.y, :attribute => :y,
                                       :duration => duration)

        if @variation.is_a? Ray::Vector3
          @animations << float_variation(:of => @variation.z, :attribute => :z,
                                         :duration => duration)
        end

        @animations.each { |anim| anim.start(@current_value) }
      end

      def pause_animation
        @animations.each { |anim| anim.pause }
      end

      def resume_animation
        @animations.each { |anim| anim.resume }
      end

      def update_target
        @animations.each { |anim| anim.update }
        target.send("#{@attribute}=", @current_value)
      end

      # @return [VectorVariation] The opposite vector variation.
      def -@
        if @initial_value
          vector_variation(:to   => @initial_value.dup,
                           :from => @initial_value + @variation,
                           :attribute => @attribute,
                           :duration => duration)
        else
          vector_variation(:of => -@variation, :attribute => @attribute,
                           :duration => duration)
        end
      end

      attr_accessor :current_value

      attr_reader :initial_value
      attr_reader :variation
      attr_reader :attribute

      def last_value
        @initial_value + @variation if @initial_value
      end

      def inspect
        hash = {
          :duration => duration,
          :running => running?,
          :attribute => attribute,
          :target => target,
          :progression => progression,
          :at => current_value
        }

        if @initial_value
          hash.merge!(:from => initial_value, :to => last_value)
        else
          hash.merge!(:of => variation)
        end

        "vector_variation(#{hash.inspect})"
      end
    end
  end

  module Helper
    # Same as #vector_variation, but :attribute is set to :pos.
    def translation(opts)
      vector_variation(opts.merge(:attribute => :pos))
    end

    # Same as #vector_variation, but :attribute is set to :scale.
    def scale_variation(opts)
      vector_variation(opts.merge(:attribute => :scale))
    end

    # Same as #vector_variation, but :attribute is set to :sheet_pos.
    # This can be used to animate a sprite while it is moving.
    def sprite_animation(opts)
      vector_variation(opts.merge(:attribute => :sheet_pos))
    end
  end
end
