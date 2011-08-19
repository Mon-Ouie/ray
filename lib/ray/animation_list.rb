module Ray
  # Animations lists are collections of animations, updating them and
  # removing all the animations that are done running. As this the only thing
  # you'll need to do most of the time, you can just push an animation at the
  # end of it and let {Scene} do the rest of the work.
  #
  # @example
  #   animations << color_variation(:from => [255, 0, 0], :to => [0, 255, 0],
  #                                 :duration => 3).start(sprite)
  #
  class AnimationList
    include Enumerable
    include Ray::PP

    def initialize
      @animations = []
    end

    # @return [true, false] True if the animation list is empty
    def empty?
      @animations.empty?
    end

    # @param [Ray::Animation] elem An animation to add to the animation list.
    def <<(elem)
      @animations << elem
      self
    end

    # Updates all the animations
    def update
      @animations.each(&:update)
      self
    end

    # Removes animations that are no more in use
    def remove_unused
      @animations.reject! do |anim|
        !anim.running? && !anim.paused?
      end

      self
    end

    # @yield Iterates over all of the animations.
    # @yieldparam [Ray::Animation] anim An animation.
    def each(&block)
      @animations.each(&block)
      self
    end

    # @return [Array<Ray::Animation>]
    attr_reader :animations

    alias to_a animations

    def inspect
      "#{self.class}#{@animations.inspect}"
    end

    def pretty_print(q)
      pretty_print_attributes q, ["animations"]
    end
  end
end
