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

    # Updates all the animations, and removes those that are finished.
    def update
      @animations.reject! do |anim|
        anim.update
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

    def inspect
      "#{self.class}#{@animations.inspect}"
    end
  end
end
