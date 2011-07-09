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
  #     # Do draw ing here
  #   end
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
  # This defaults to 60.
  #
  # @see Ray::DSL::EventTranslator
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
      @scene_register_block   = block
      @scene_always_block     = nil
      @scene_render_block     = nil
      @scene_clean_block      = nil

      @scene_loops_per_second = 60

      @scene_animations       = Ray::AnimationList.new
    end

    def register_events
      if @scene_register_block
        instance_exec(@scene_arguments,  &@scene_register_block)
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

    # Runs until you exit the scene.
    # This will also raise events if the mouse moves, ... allowing you
    # to directly listen to a such event.
    def run
      until @scene_exit
        loop_start = Time.now

        collect_events

        @scene_always_block.call if @scene_always_block
        listener_runner.run
        @scene_animations.update

        @scene_window.clear(Ray::Color.none)
        render @scene_window
        @scene_window.update

        if @scene_loops_per_second
          ellapsed_time = Time.now - loop_start
          time_per_loop = 1.0 / @scene_loops_per_second

          sleep(time_per_loop - ellapsed_time) if ellapsed_time < time_per_loop
        end
      end

      clean_up
    end

    # Runs another scene over the current one.
    # This method will return when the scene is done running.
    def run_scene(name, *args)
      scene_list = SceneList.new(game)
      scene_list.push(name, *args)

      event_runner = DSL::EventRunner.new

      old_event_runner = game.event_runner
      old_scene_list   = game.scenes

      game.event_runner = event_runner
      game.scenes = scene_list

      begin
        game.run
      ensure
        game.event_runner = old_event_runner
        game.scenes       = old_scene_list
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

    # Registers the block to draw the scene.
    #
    # If no block is given, renders the scen on the image passed as
    # an argument.
    #
    # @yield [window] Block to render this scene.
    # @yieldparam [Ray::Image] window The window you should draw on
    def render(win = nil, &block)
      if block_given?
        @scene_render_block = block
      else
        @scene_render_block.call(win) if @scene_render_block
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
        @scene_clean_block.call if @scene_clean_block
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

    alias :frames_per_second :loops_per_second
    alias :frames_per_second= :loops_per_second=

    # The arguments passed to the scene with push_scene
    attr_accessor :scene_arguments

    # @return [Ray::AnimationList] An animation list automatically updated by
    #   the scene.
    def animations
      @scene_animations
    end

    private
    def collect_events
      window.each_event do |ev|
        raise_event(*DSL::EventTranslator.translate_event(ev))
      end
    end
  end
end
