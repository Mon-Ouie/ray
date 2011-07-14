module Ray
  class Animation
    # This class allows to use a single object to apply several kind of
    # animations to a same target.
    #
    # Note combining two animations affecting the same attribute usually won't
    # work.
    #
    # Thus:
    #   translation(:of => [10, 10], :duration => 2) +
    #     translation(:of => [10, 10], :duration => 2)
    # Is not the same as:
    #   translation(:of => [20, 20], :duration => 2)
    #
    #
    class Combination < Animation
      register_for :animation_combination

      def setup(*args)
        @animations = args
        self.duration = @animations.map(&:duration).max || 0
      end

      def setup_target
        @animations.each { |anim| anim.start(target) }
      end

      def update_target
        @animations.each { |anim| anim.update }
      end

      def pause_animation
        @animations.each { |anim| anim.pause }
      end

      def resume_animation
        @animations.each { |anim| anim.resume }
      end

      # Adds several animations to this combination.
      def push(*animations)
        @animations.concat(animations)
        max_duration = animations.map(&:duration).max || 0
        self.duration = max_duration if max_duration > duration

        self
      end

      alias :<< :push

      # (see Ray::Animation#+)
      def +(animation)
        animation_combination(*(@animations + [animation]))
      end

      # (see Ray::Animation#-)
      def -(animation)
        animation_combination(*(@animations + [-animation]))
      end

      # @return [Ray::Animation::Combination] Combination of the opposite of
      #   each animation stored in this object.
      def -@
        animation_combination(*@animations.map { |anim| -anim })
      end

      # @return [Array<Ray::Animation>]
      attr_reader :animations
    end
  end
end
