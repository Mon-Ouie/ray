module Ray
  # Games are used to manage different scenes. They also init Ray by creating a
  # window.
  #
  # == Creating a Game
  # There are several ways of doing this.
  # Using a block:
  #   Ray::Game.new("my game") do
  #     ...
  #   end
  #
  # Using the instance directly:
  #   game = Ray::Game.new("my game")
  #   ...
  #   game.run
  #
  # Subclassing:
  #   class Game < Ray::Game
  #     def initialize
  #       super("my game")
  #       ...
  #     end
  #   end
  #
  #   Game.new.run
  #
  # == Registring scenes to a Game
  # Games need the scenes they use to be registered. The most obvious way to
  # do it is to use scene with a block:
  #   scene :game do
  #     # See Ray::Scene
  #   end
  #
  # You may also call it to register a subclass of Ray::Scene
  #   scene(:game, GameScene)
  # Which is the same as:
  #   GameScene.bind(self) # Assuming GameScene's scene_name is set to :game
  #
  # == Managing the scene stack
  # You can push a scene to the game:
  #   push_scene :game
  # When #run will be called, it will show the scene :game. Notice that, if you
  # push more than one scene, only the last one will be seen directly. However,
  # if you remove it later, the previous scene will be shown.
  #
  # You can thus also remove a scene from your stack:
  #   pop_scene # Removes the last scene
  #
  # exit is not exactly the same: it will ask the scene to quit before doing this.
  # exit! will do something totally different: completely kill the game.
  #
  # == Handling events
  # Games can listen to events just like scenes. Since the event runner will change
  # often, it needs to register every time it changes it. You can pass a block to
  # the register method:
  #   register do
  #     on :some_event do some_stuff end
  #   end
  #
  # You may also want to override register in suclasses:
  #   def register
  #     on :some_event do some_stuff end
  #   end
  #
  class Game
    include Ray::Helper

    # Creates a new game.
    #
    # @yield If a block is given, it is instance evaluated. #run will then be
    #   called.
    #
    # @option opts [Ray::Window] :window (nil) If set, this window will be used
    #   instead of creating a new one
    # @option opts [Ray::Vector2, #to_vector2] :size ([640, 480]) Size of the
    #  window.
    # @option opts [true, false] :fullscreen (false) True to make the window
    #   fullscreen.
    # @option opts [true, false] :resizable (false) True to make the window
    #   resizable.
    # @option opts [true, false] :no_frame (false) True to create a window
    #   with no decorations.
    def initialize(title, opts = {}, &block)
      @game_registered_scenes = {}
      @game_scenes = SceneList.new(self)
      @game_register_block = nil
      @game_exited = false
      @game_title = title

      create_event_runner

      defaults = {:size => [640, 480]}
      options = defaults.merge(opts)

      unless options[:window]
        size = options[:size].to_vector2

        @game_window = Ray::Window.new
        @game_window.open(title, size, options)
      else
        @game_window = opts[:window]
      end

      if block
        instance_eval(&block)
        run
      end
    end

    # Adds a scene to the stack using its name.
    #
    # You must call Game#scene before this. If you subclassed scene,
    # then call bind to register it:
    #   scene :something, SomeClass
    #   SomeClass.bind(self)
    #
    # @param [Symbol] scene_name The name of the scene which should be pushed
    # @param *args Arguments passed to the scene
    def push_scene(scene_name, *args)
      @game_scenes.push(scene_name, *args)
    end

    # Pops the last scene
    def pop_scene
      @game_scenes.pop
    end

    # Pops scenes while a condition is true
    #
    # @yield [scene] To determine if a scene must be popped.
    # @yieldparam [Ray::Scene] scene The scene that will be popped
    # @yieldreturn [Boolean] True to pop the next scene
    def pop_scene_while
      while yield scenes.current
        scenes.current.pop_scene # ensure exit is set to false
      end
    end

    # Pops scenes until a condition is met
    #
    # @yield (see #pop_scene_while)
    # @yieldparam scene (see #pop_scene_while)
    # @yieldreturn [Boolean] False to pop the next scene
    def pop_scene_until
      pop_scene_while { |o| !yield(o) }
    end

    # Registers a new scene with a given name. the block will be passed
    # to klass.new.
    #
    # @param [Symbol] name the name of the new scene
    # @param [Class] klass the class of the scene.
    def scene(name, klass = Scene, &block)
      scene = @game_registered_scenes[name] = klass.new(&block)

      scene.game         = self
      scene.event_runner = event_runner
      scene.window       = @game_window
      scene.name         = name
    end

    # @return [Ray::Scene] scene register for a given name
    def registered_scene(name)
      @game_registered_scenes[name]
    end

    # Runs the game until the last scene gets popped.
    # Will call Ray.stop.
    def run
      while running?
        event_runner.clear
        register

        @game_scenes.run_current
      end
    end

    # @param [true, false] True if the scene list isn't empty
    #   and the user hasn't exited from the game.
    def running?
      !@game_exited && !@game_scenes.empty?
    end

    # Registers a block to listen to events
    # Subclasses can also overrid this method to register for events.
    def register(&block)
      if block_given?
        @game_register_block = block
      else
        @game_register_block.call if @game_register_block
      end
    end

    # Removes the current scene of this game
    def exit
      @game_scenes.exit_current
    end

    # Kills the game, removing all the scenes it contains.
    def exit!
      @game_exited = true

      @game_scenes.exit_current
      @game_scenes.clear
    end

    # @return [Ray::String]
    def title
      @game_title
    end

    # @return [Ray::SceneList]
    def scenes
      @game_scenes
    end

    # @return [Ray::Window]
    def window
      @game_window
    end

    def scenes=(list)
      @game_scenes = list

      unless running?
        @game_scenes.exit_current
      end
    end

    def event_runner=(runner)
      super

      @game_registered_scenes.each do |name, scene|
        scene.event_runner = runner
      end
    end

    def inspect
      "game(#{title.inspect})"
    end
  end

  # (see Ray::Game#initialize)
  def game(title, opts = {}, &block)
    Ray::Game.new(title, opts, &block)
  end

  module_function :game
end
