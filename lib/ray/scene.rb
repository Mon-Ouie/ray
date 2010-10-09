module Ray
  # Scenes contain the main logic of a game.
  #
  # You can define a new scene using a block, which will be called every time
  # your scene is about to be used. However, you may also want to subclass
  # Ray::Scene. When doing this, you'll probably want to override the register
  # method:
  #   def register
  #     on :some_event do some_stuff end
  #   end
  #
  # Another method is called before register: setup. Putting code in register or
  # in setup doesn't matter, but setting the scene up inside register method
  # seems (and, indeed, is) inappropriate. You can override it:
  #   def setup
  #     @sprite = sprite("image.png")
  #   end
  #
  # You can indicate how your scene should be rendered there:
  #    render do |win|
  #      # Do drawing here
  #    end
  #
  # Or you can override render:
  #   def render(win)
  #     # Do drawing here
  #   end
  #
  # Notice win is not filled with an empty color when render is called, i.e.
  # it still contains the frame which appears to the user.
  #
  # Also, scenes are rendered lazily: only once when the scene is created,
  # and then every time need_render! is called.
  #
  # Once your scene is loaded, you'll probably want to clean it up (set some
  # instance variables to nil so they can be garbaged collected for instance).
  # You can do that by passing a block to clean_up:
  #   clean_up do
  #     @some_big_resource = nil
  #   end
  #
  # Or by overriding it:
  #   def clean_up
  #     @some_big_resource = nil
  #   end
  #
  # == Managing the stack of scenes
  # exit is called when you want to stop running the scene, but not to remove
  # the last scene from the stack. It is useful if you want to push a new
  # scene. Hence Ray::Scene#push_scene will call exit.
  #
  # exit! (or pop_scene), on the other hand, is used to go back to the previous
  # scene in the hierarchy
  #
  # == Sending informations to a scene
  # Scenes may need some arguments to work. You can pass those in push_scene:
  #   push_scene(:polygon, 6, Ray::Color.red)
  # Then you can use them with scene_arguments:
  #   scene :polygon do
  #     sides, color = scene_arguments
  #     # ...
  #   end
  #
  # They are also passed to #setup:
  #   def setup(sides, color)
  #     # ...
  #   end
  #
  # == Limiting the loop rate
  # You can prevent a scene from always running by using #loops_per_second=:
  #   self.loops_per_second = 30 # will sleep some time after each loop
  class Scene
    include Ray::Helper

    class << self
      # Registers a scene to a game object, used for subclasses.
      def bind(game)
        game.scene(scene_name, self)
      end

      # @overload scene_name
      #   @return [Symbol] the name of the scene
      # @overload scene_name(value)
      #   Sets the name of the scene
      def scene_name(val = nil)
        @scene_name = val || @scene_name
      end
    end

    scene_name :scene

    # Creates a new scene. block will be instance evaluated when
    # this scene becomes the current one.
    def initialize(&block)
      @scene_register_block = block
    end

    def register_events
      @scene_held_keys = []

      on :key_press do |key, mod|
        @scene_held_keys << key
      end

      on :key_release do |key, mod|
        @scene_held_keys.reject! { |i| i == key }
      end

      if @scene_register_block
        instance_eval(&@scene_register_block)
      else
        register
      end

      @scene_exit = false
    end

    # Override this method in subclasses to setup the initial state
    # of your scene.
    def setup(*args)
    end

    # Override this method in subclasses to register your own events
    def register
    end

    # @param [Symbol, Integer] val A symbol to find the key (its name)
    #                              or an integer (Ray::Event::KEY_*)
    #
    # @return [true, false] True if the user is holding key.
    def holding?(val)
      if val.is_a? Symbol
        val = key(val)
        @scene_held_keys.any? { |o| val === o }
      elsif val.is_a? DSL::Matcher
        @scene_held_keys.any? { |o| val === o }
      else
        @scene_held_keys.include? val
      end
    end

    # Runs until you exit the scene.
    # This will also raise events if the mouse moves, ... allowing you
    # to directly listen to a such event.
    def run
      until @scene_exit
        loop_start = Time.now

        DSL::EventTranslator.translate_event(Ray::Event.new).each do |args|
          raise_event(*args)
        end

        @scene_always_block.call if @scene_always_block

        listener_runner.run

        if @scene_need_render
          @scene_need_render = false

          if @scene_render_block
            @scene_render_block.call(@scene_window)
          else
            render(@scene_window)
          end

          @scene_window.flip
        end

        if @scene_loops_per_second
          ellapsed_time = Time.now - loop_start
          time_per_loop = 1.0 / @scene_loops_per_second

          sleep(time_per_loop - ellapsed_time) if ellapsed_time < time_per_loop
        end
      end

      if @scene_clean_block
        @scene_clean_block.call
      else
        clean_up
      end
    end

    # Exits the scene, but does not pop the scene.
    #
    # You may want to call this if you pushed a new scene, to switch to
    # the new scene.
    def exit
      @scene_exit = true
    end

    # Exits the scene and pops it (may not work as expected if the current
    # scene is not the last one)
    def exit!
      exit
      game.pop_scene
    end

    # Registers a block to be excuted as often as possible.
    def always(&block)
      @scene_always_block = block
    end

    # Marks the scene should be redrawn.
    def need_render!
      @scene_need_render = true
    end

    # Registers the block to draw the scene.
    #
    # Does nothing if no block is given, this method being called if you
    # didn't register any render block. You can thus override it in subclasses
    # instead of providing a block to it.
    #
    # @yield [window] Block to render this scene.
    # @yieldparam [Ray::Image] window The window you should draw on
    def render(win = nil, &block)
      if block_given?
        @scene_render_block = block
      else
        # Do nothing
      end
    end

    # Pushes a scene in the stack, and exits that one
    def push_scene(scene, *args)
      game.push_scene(scene, *args)
      exit
    end

    # Cleans the scene or registers a block to clean it.
    def clean_up(&block)
      if block_given?
        @scene_clean_block = block
      else
        # Do nothing
      end
    end

    def inspect
      "#<#{self.class} game=#{self.game.inspect}>"
    end

    alias :pop_scene :exit!

    def game
      @scene_game
    end

    def game=(val)
      @scene_game = val
    end

    def window
      @scene_window
    end

    def window=(val)
      @scene_window = val
    end

    def loops_per_second
      @scene_loops_per_second
    end

    def loops_per_second=(val)
      @scene_loops_per_second = val
    end

    # The arguments passed to the scene with push_scene
    attr_accessor :scene_arguments
  end
end
