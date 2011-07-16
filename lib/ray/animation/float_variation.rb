module Ray
  class Animation
    # Animates the transition of a float to another one.
    # Registered as #float_variation.
    class FloatVariation < Animation
      register_for :float_variation

      # @option opts [Float] :of Variation of the value. May be negative.
      #   Required unless :to and :from are set.
      # @option opts [Float] :from Initial value of attribute.
      # @option opts [Float] :to Last value of attribute.
      # @option opts [Symbol] :attribute Attribute of the target to change.
      # @option opts [Float] :duration Duration in seconds.
      def setup(opts)
        if opts[:of]
          @initial_value = nil
          @variation = opts[:of]
        else
          @initial_value = opts[:from]
          @variation = opts[:to] - @initial_value
        end

        @attribute = opts[:attribute]

        self.duration = opts[:duration]
      end

      def setup_target
        unless @initial_value
          @current_value = target.send(@attribute)
        else
          @current_value = @initial_value
        end
      end

      def update_target
        increase = progression * @variation
        target.send("#{@attribute}=", @current_value + increase)
      end

      def end_animation
        target.send("#{@attribute}=", @current_value + @variation)
      end

      # @return [FloatVariation] The opposite variation.
      def -@
        if @initial_value
          float_variation(:from => @initial_value + @variation,
                          :to => @initial_value,
                          :attribute => @attribute,
                          :duration => duration)
        else
          float_variation(:of => -@variation, :attribute => @attribute,
                          :duration => duration)
        end
      end

      # @return [Symbol] Attribute modified by the animation.
      attr_reader :attribute

      # @return [Float] Initial value.
      attr_reader :initial_value

      # @return [Float] Variation between the initial value and the last value.
      attr_reader :variation
    end
  end

  module Helper
    # @return [Ray::Animation::FloatVariation] Same as #float_variation, but
    #   attribute is set to angle.
    def rotation(opts)
      float_variation(opts.merge(:attribute => :angle))
    end
  end
end
