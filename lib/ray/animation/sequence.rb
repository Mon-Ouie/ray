module Ray
  class Animation
    # Stores an array of animations that will be executed sequentially.
    #
    # Registered as animation_sequence.
    class Sequence < Animation
      register_for :animation_sequence

      # @param [Array<Animation>] animations Animation to execute
      def setup(*animations)
        @animations   = animations
        @current_anim = nil

        self.duration = @animations.inject(0) { |sum, a| sum + a.duration }
      end

      def setup_target
        @animations.each { |anim| anim.event_runner = event_runner }

        @animations.each_cons(2) do |prev, succ|
          on :animation_end, prev do
            @current_anim = succ
            succ.start target
          end
        end

        @current_anim = @animations.first

        @current_anim.start target
      end

      def update_target
        @current_anim.update
      end

      def pause_animation
        @current_anim.pause
      end

      def resume_animation
        @current_anim.resume
      end

      # Adds several animations to the sequence.
      def push(*animations)
        @animations.concat(animations)
        self.duration += animations.inject(0) { |sum, a| sum + a.duration }

        self
      end

      alias :<< :push

      # @return [Ray::Animation::Sequence] Animation sequence, calling reversed
      #   animations in reversed order.
      def -@
        anims = @animations.reverse.map { |anim| -anim }
        animation_sequence(*anims)
      end

      # @return [Array<Ray::Animation>]
      attr_reader :animations
    end
  end
end
