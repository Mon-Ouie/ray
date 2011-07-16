module Ray
  class Animation
    # Animation occurring by running a block.
    # Registered as #block_animation.
    # @example
    #   block_animation :duration => 10, :block => proc { |target, progression|
    #     p target => progression
    #   }
    class BlockAnimation < Animation
      register_for :block_animation

      # @option opts [#call] :block Object which will be called with the target
      #   and a progression between 0 and 1.
      # @option opts [#call] :proc Same as :block.
      # @option opts [true, false] :reversed (false) True to make the
      #   progression decrease from 1 to 0.
      # @option opts [Float] :duration Duration in seconds.
      def setup(opts)
        @block    = opts[:block] || opts[:proc]
        @reversed = opts.key?(:reversed) ? opts[:reversed] : false

        self.duration = opts[:duration]
      end

      def update_target
        @block.call(target, @reversed ? 1 - progression : progression)
      end

      # @return [BlockAnimation] Reversed animation.
      def -@
        block_animation(:block => @block, :reversed => !@reversed,
                        :duration => @duration)
      end
    end
  end
end
