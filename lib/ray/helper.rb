module Ray
  # Module including many helpful modules and methods.
  module Helper
    include Ray::DSL::EventRaiser
    include Ray::DSL::EventListener

    include Ray::Matchers

    # Sets the event runner for this object.
    def event_runner=(value)
      self.listener_runner = value
      self.raiser_runner   = value
    end

    # @return [DSL::EventRunner] The event runner used to handle event.
    def event_runner
      listener_runner
    end

    # Creates an event runner for this object
    def create_event_runner
      self.event_runner = Ray::DSL::EventRunner.new
    end

    # @see Ray::ImageSet.[]
    def image(name)
      Ray::ImageSet[name]
    end

    # @see Ray::Sprite#initialize
    def sprite(image, opts = {})
      Ray::Sprite.new(image, opts)
    end

    # @see Ray::Channel#initialize
    def channel(id)
      Ray::Channel.new(id)
    end

    # @see Ray::FontSet.[]
    def font(name, size)
      Ray::FontSet[name, size]
    end
  end
end
