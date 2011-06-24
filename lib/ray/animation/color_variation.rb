module Ray
  class Animation
    # This animates the transition from a color to another one.
    # Registered as #color_variation.
    class ColorVariation < Animation
      register_for :color_variation

      # @option opts [Array] :of [r, g, b, a] Variation for the color.
      #   Required unless :from and :to are set.
      # @option opts [Ray::Color] :from Initial value for the color.
      # @option opts [Ray::Color] :to Last value of the color.
      # @option opts [Symbol] :attribute (:color) Attribute of the target to change.
      # @option opts [Float] :duration Duration in seconds.
      def setup(opts)
        if opts[:of]
          @current_value = @initial_value = nil
          @variation = opts[:of].to_a
        else
          @initial_value = opts[:from].to_color

          value_array = @initial_value.to_a

          @variation = opts[:to].to_a.each_with_index.map do |o, i|
            o - value_array[i]
          end
        end

        @attribute = opts[:attribute] || :color

        self.duration = opts[:duration]
      end

      def setup_target
        unless @initial_value
          @current_value = target.send(@attribute)
        else
          @current_value = @initial_value.dup
        end

        @animations = []
        @animations << float_variation(:of => @variation[0], :attribute => :r,
                                       :duration => duration)
        @animations << float_variation(:of => @variation[1], :attribute => :g,
                                       :duration => duration)
        @animations << float_variation(:of => @variation[2], :attribute => :b,
                                       :duration => duration)
        @animations << float_variation(:of => @variation[3], :attribute => :a,
                                       :duration => duration)

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

      # @return [ColorVariation] Opposite color variation.
      def -@
        if @initial_value
          color_variation(:from => last_value, :to => @initial_value,
                          :attribute => @attribute, :duration => duration)
        else
          rev_variation = @variation.map { |o| -o }
          color_variation(:of => rev_variation, :attribute => @attribute,
                          :duration => duration)
        end
      end

      # @return [Ray::Color] Current color.
      attr_accessor :current_value

      # @return [Ray::Color] Initial color.
      attr_reader :initial_value

      # @return [Array<Integer>] Variation between initial and last value.
      attr_reader :variation

      # @return [Symbol] Attribut modified by the animation.
      attr_reader :attribute

      # @return [Ray::Color] Last color.
      def last_value
        if @initial_value
          initial_array = @initial_value.to_a

          Ray::Color.new(*initial_array.each_with_index.map { |o, i|
                           o + @variation[i]
                         })
        end
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

        "color_variation(#{hash.inspect})"
      end
    end
  end
end
