module Ray
  # Animations are changes progressively applied to an object.
  # This class should be subclassed by the actual animations.
  #
  # == Creating an animation object
  # Subclasses create a helper method in Ray::Helper to create the
  # animation:
  #   translation(:from => [0, 0], :to => [120, 120], :duration => 30)
  #   rotation(:from => 10, :to => 360, :duration => 2)
  #
  # Then, the animation isn't running yet. It is needed to call #start
  # with the target of the animation:
  #   @translation.start some_sprite
  #
  # You'd then need to call #update regularily to apply the changes.
  # For instance, in a scene, you'd do:
  #   always { @translation.update }
  #
  # (See also {AnimationList})
  #
  # Animations can be paused (#pause) and resumed (#resume) if needed.
  #
  # They also raise an :animation_end event you could use to call another event:
  #   on :animation_end, @translation do
  #     @rotation.start some_sprite
  #   end
  #
  # == Creating a custom animation class
  #   class MyOwnAnimation < Ray::Animation
  #     # Creates the helper method
  #     register_for :my_own_animation
  #
  #     # hash is the argument passed to the helper method.
  #     # target is still nil.
  #     def setup(hash)
  #       @some_param = hash[:param]
  #       self.duration = 10 # You *need* to set this
  #     end
  #
  #     # the animation has just started, target is set.
  #     def setup_target
  #     end
  #
  #     # update has been called
  #     def update_target
  #       # progression is a float between 0 and 100.
  #       target.x += (@some_param * progression / 100)
  #     end
  #
  #     # the animation has just ended.
  #     def end_animation
  #     end
  #
  #     # Not required. You can get an animation cancelling the first one
  #     # using this in most animation classes.
  #     def -@
  #       my_own_animation(:param => -@some_param)
  #     end
  #   end
  class Animation
    include Ray::Helper

    def self.register_for(name)
      klass = self

      Ray::Helper.send :define_method, name do |*args|
        animation = klass.new
        animation.event_runner = event_runner
        animation.setup(*args)

        animation
      end
    end

    def initialize
      @duration = 0

      @start_time = nil
      @pause_time = nil
      @end_time   = nil

      @running = false

      @target = nil

      @reversed = false
    end

    # Starts the animation.
    # @param [Object] on The target of the animation
    # @return self
    def start(on)
      @target = on

      @start_time = Time.now
      @end_time   = @start_time + duration

      @running = true

      setup_target

      self
    end

    # Override this in subclasses.
    # Will be called by the helper method (when target is still nil).
    def setup(*args)
      raise NotImplementedError
    end

    # Method called when the animation is started.
    # Override it if needed.
    def setup_target
    end

    # Pauses the animation.
    def pause
      @pause_time = Time.now
      @running    = false

      pause_animation
    end

    # Resumes from the pause.
    # The time when the animation should end is also updated.
    def resume
      return unless @pause_time

      # Reset @start_time to compute the progression more easily.
      @end_time   = Time.now + duration - (@pause_time - @start_time)
      @start_time = @end_time - duration
      @pause_time = nil

      @running = true

      resume_animation
    end

    # Updates the target if the animation is running.
    # May also end the animation.
    def update
      if running?
        update_target

        if Time.now >= @end_time
          @running  = false
          @end_time = nil

          end_animation
          raise_event :animation_end, self if raiser_runner
        end
      end
    end

    # Override this method to apply changes to the target.
    def update_target
      raise NotImplementedError
    end

    # Override this method to do something at the end of the animation.
    def end_animation
    end

    # Override this if you need to do something when the animation is paused
    # (for instance, paussing animations used by this object).
    def pause_animation
    end

    # Override this if you need to do something when the animation is resumed
    def resume_animation
    end

    # @return [Float, nil] The progression of the animation, between 0 and 100.
    #   Nil if the animation isn't running.
    def progression
      if running?
        if @duration.zero?
          100.0
        else
          ret = 100 * (Time.now - @start_time) / duration
          ret > 100 ? 100.0 : ret
        end
      end
    end

    # @return [Ray::Animation::Combination] Combination of self and the
    #   argument.
    def +(other)
      animation_combination(self, other)
    end

    # @return [Ray::Animation::Combination] Combination of self and the oppsite
    #  of the argument.
    def -(other)
      animation_combination(self, -other)
    end

    # @return [true, false] True if the animation is running
    def running?; @running ; end

    # @return [true, false] True if the animation is paused
    def paused?; not @pause_time.nil?; end

    # @return [Object] The target of the animation.
    attr_reader :target

    # @return [Float] Duration of the animation, in seconds.
    attr_accessor :duration

    # @return [Time] Time when the animation should end.
    attr_reader :end_time
  end
end

require 'ray/animation/block_animation'
require 'ray/animation/float_variation'
require 'ray/animation/vector_variation'
require 'ray/animation/color_variation'
require 'ray/animation/circular_motion'
require 'ray/animation/combination'
