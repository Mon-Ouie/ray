module Ray
  # Games are used to manage different scenes. They also init Ray and create a
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
  # Games need the scenes they use to be registred. The most obvious way to
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
    # You can poss all the argument you would pass to create_window,
    # except width and height which should be given in :video_mode, like this :
    #   Ray::Game.new('hello', :video_modes = %w(480x272 640x480))
    #
    # It will try to get the biggest resolution available (so it will most
    # likely choose 640x480).
    #
    # If a block is passes, it is instance evaluated, and then the game is
    # directly run.
    #
    # This methods creates a new window and inits Ray.
    def initialize(title, hash = {}, &block)
      @game_registred_scenes = {}
      @game_scenes = []

      defaults = {
        :double_buf  => true,
        :bpp         => 32,
        :hw_surface  => true,
        :sw_surface  => false,
        :video_modes => %w(480x272 640x480)
      }

      options = defaults.merge(hash)

      common_settings = {
        :sw_surface => options[:sw_surface],
        :hw_surface => options[:hw_surface],
        :bpp        => options[:bpp] || options[:bits_per_pixel],
        :async_blit => options[:async_blit],
        :double_buf => options[:double_buf],
        :fullscreen => options[:fullscreen],
        :resizable  => options[:resizable],
        :no_frame   => options[:no_frame]
      }

      modes = options[:video_modes].map { |m| m.split('x').map { |i| i.to_i } }

      # The biggest resolution is privileged
      modes = modes.sort_by { |(w, h)| w * h }.map do |(w, h)|
        common_settings.merge(:w => w, :h => h)
      end

      Ray.init

      last_mode = modes.select { |mode| Ray.can_use_mode? mode }.last
      raise ArgumentError, "No valid mode found" unless last_mode

      if @game_title = options[:title]
        Ray.window_title = @game_title
        Ray.text_icon    = @game_title
      end

      if icon = options[:icon]
        Ray.icon = icon.is_a?(Ray::Image) ? icon : icon.to_image
      end

      @game_window = Ray.create_window(last_mode)

      if block
        instance_eval(&block)
        run
      end
    end

    # Adds a scene to the stack by its name.
    #
    # You must call Game#scene before this. If you subclassed scene,
    # then call bind to register it.
    #
    # @param [Symbol] scene_name The name of the scene which should be pushed
    # @param *args Arguments passed to the scene
    def push_scene(scene_name, *args)
      scene = @game_registred_scenes[scene_name]
      raise ArgumentError, "Unknown scene #{scene_name}" unless scene

      @game_scenes << scene
      @game_scene_arguments = args
    end

    # Pops the last scene.
    def pop_scene
      @game_scenes.delete_at(-1)
    end

    # Registers a new scene with a given name. the block will be passed
    # to klass.new.
    #
    # @param [Symobl] name the name of the new scene
    # @param [Class] klass the class of the scene.
    def scene(name, klass = Scene, &block)
      @game_registred_scenes[name] = klass.new(&block)
    end

    # Runs the game until the last scene gets popped, and stop ray.
    def run
      until @game_scenes.empty?
        create_event_runner
        if @game_register_block
          @game_register_block.call
        else
          register
        end

        @game_scenes.each do |scene|
          scene.game            = self
          scene.window          = @game_window
          scene.event_runner    = event_runner
          scene.scene_arguments = @game_scene_arguments
        end

        scene = @game_scenes.last

        scene.setup(*@game_scene_arguments)
        scene.register_events
        scene.need_render!
        scene.run
      end

      Ray.stop
    end

    # Registers a block to listen to events
    # Subclasses can also overrid this method to register for events.
    def register(&block)
      if block_given?
        @game_register_block = block
      else
        # Do nothing
      end
    end

    # Removes the current scene of this game
    def exit
      return if @game_scenes.empty?

      @game_scenes.last.exit
      pop_scene
    end

    # Kills the game, removing all the scenes of this game
    def exit!
      return if @game_scenes.empty?

      @game_scenes.last.exit
      @game_scenes.clear
    end

    def title
      @game_title
    end

    def inspect
      "game(#{title.inspect})"
    end
  end

  # @see Ray::Game.new
  def self.game(title, opts = {}, &block)
    Ray::Game.new(title, opts, &block)
  end
end
