module Ray
  # A game is represented as a stack of scenes, running until there are no more
  # scenes.
  class Game
    include Ray::Helper

    # Creates a new game.
    #
    # You can poss all the argument you would pass to create_window,
    # except width and height which should be given in :video_mode, like this :
    #   Ray::Game.new(:video_modes = %w(480x272 640x480))
    #
    # It will try to get the biggest resolution available (so it will most
    # likely choose 640x480).
    #
    # If a block is passes, it is instance evaluated, and then the game is
    # directly run.
    #
    # This methods creates a new window and inits Ray.
    def initialize(hash = {}, &block)
      @game_registred_scenes = {}
      @game_scenes = []

      defaults = {
        :double_buf => true,
        :bpp        => 32,
        :hw_surface => true,
        :sw_surface => false
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
        Ray.icon = Ray.convert(icon, :image)
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
    def push_scene(scene_name)
      scene = @game_registred_scenes[scene_name]
      raise ArgumentError, "Unknown scene #{scene_name}" unless scene

      @game_scenes << scene
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
        if @game_register
          @game_register.call
        else
          register
        end

        @game_scenes.each do |scene|
          scene.game         = self
          scene.window       = @game_window
          scene.event_runner = event_runner
        end

        scene = @game_scenes.last

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
        @game_register = block
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

    attr_reader :title

    def inspect
      "game(#{title.inspect})"
    end
  end
end

module Kernel
  # Creates a new game, with the given title.
  # @see Ray::Game#initialize
  def game(title, opts = {}, &block)
    opts = {:title => title, :video_modes => %w(480x272 640x480)}.merge(opts)
    Ray::Game.new(opts, &block)
  end

  module_function :game
end
