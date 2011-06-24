module Ray
  #
  # == Event handling
  # This module is used to allow both raising events and handling thereof.
  # You can raise them simply with raise_event:
  #   raise_event(:event_name, arugments)
  # (You can add as many extra arguments as you want/need to)
  #
  # Those events can trigger a block.
  #   on :event_name do
  #     # Do something useful
  #   end
  #
  # If you want your block to be triggered only when a condition is
  # met, you can pass extra arguments to on. They will be compared
  # to those that were passed to raise_event using === or == if
  # === returned false.
  #   on :event_name, /foo/ do |str| puts str end
  #   raise_event "foobar" # Will trigger the above
  #
  # Handlers can be grouped:
  #   event_group :name do
  #     # Register for some events
  #   end
  #
  # Notice event groups cannot be nested. The following code:
  #   event_group :name do
  #     event_group :foo do
  #       # ...
  #     end
  #
  #     # ...
  #   end
  # Will create two totally unrelated event groups (:name and :foo).
  #
  # An event group is enabled by default, but it can be disabled:
  #   disable_event_group :name
  # And re-enabled afterwards:
  #   enable_event_group :name
  # It is also possible to remove the handlers that belong to a group
  # when they're not needed anymore:
  #   remove_event_group :name
  #
  # == Shared resources
  #
  # This module contains helper methods that provide access to shared resources,
  # i.e. they will always return the same object until the cache is cleared:
  #   obj = image "test.png"
  #   other_obj = image "test.png"
  #   obj.equal? other_obj # => true
  #
  # Notice different resources sets are used. This allows to get resources from
  # other places than from the disk. For instance, Ray provides a way to get
  # resources from the network using open-uri:
  #   obj = image "http://www.example.com/some_image.png" # Download, takes some time
  #   other_image = image "http://www.example.com/some_image.png" # Doesn't download
  #   obj.equal? other_image
  #
  # You can define your own resource sets pretty easily:
  #   Ray.image_set(/some (regexp)/) do |capture| # Captures are yielded
  #     # Block that returns an image
  #   end
  #
  # Be careful when using a shared resource. Call #dup instead of mutating it (even
  # though that would not raise an error).
  #
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

    # Enables an event group
    def enable_event_group(name)
      event_runner.enable_group(name)
    end

    # Disables an event group
    def disable_event_group(name)
      event_runner.disable_group(name)
    end

    # Removes all the handlers belonging to an event group
    def remove_event_group(name)
      event_runner.remove_group(name)
    end

    module_function

    # (see Ray::ImageSet.[])
    def image(name)
      Ray::ImageSet[name]
    end

    # (see Ray::ImageTarget#initialize)
    def image_target(img = nil, &block)
      Ray::ImageTarget.new(img, &block)
    end

    # (see Ray::Sprite#initialize)
    def sprite(image, opts = {})
      Ray::Sprite.new(image, opts)
    end

    # (see Ray::SoundBufferSet.[])
    def sound_buffer(file)
      Ray::SoundBufferSet[file]
    end

    # (see Ray::Sound#initialize)
    def sound(file)
      Ray::Sound.new file
    end

    # (see Ray::Music#initialize)
    def music(file)
      Ray::Music.new(file)
    end

    # (see Ray::FontSet.[])
    def font(name)
      Ray::FontSet[name]
    end

    # (see Ray::Text#initialize)
    def text(content, opts = {})
      Ray::Text.new(content, opts)
    end

    # @param [Symbol, Key, Integer] val A symbol to find the key (its name),
    #   a Key object, or one of the KEY_* constant.
    # @return [true, false] True if the user is holding key.
    def holding?(val)
      if val.is_a? Integer
        window.input.holding? val
      else
        key(val.to_sym).to_a.any? do |key|
          window.input.holding? key
        end
      end
    end
  end
end
