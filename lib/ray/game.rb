module Ray
  # a game is represented as a stack of scenes : you register scenes,
  # and then you push and pop them until the stack is empty.
  #
  # The game will run eerything when it is created, and free it when
  # it is done running.
  class Game
    # Creates a new game.
    # You can poss all the argument you would pass to create_window,
    # except width and height which should be given in :video_mode, like this :
    #   Ray::Game.new(:video_modes = %w(480x272 640x480))
    #
    # It will try to get the biggest resolution available (so it will most
    # likely choose 640x480).
    #
    # If a block is passes, it is instance evaluated, and then the game is
    # directly run.
    def initialize(hash = {}, &block)
      @registred_scenes = {}
      @scenes = []

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

      @window = Ray.create_window(last_mode)

      if block
        instance_eval(&block)
        run
      end
    end

    # Adds a scene to the stack by its name.
    # You must call Game#scene before this. If you subclassed scene,
    # then call bind to register it.
    def push_scene(scene_name)
      scene = @registred_scenes[scene_name]
      raise ArgumentError, "Unknown scene #{scene_name}" unless scene

      @scenes << scene
    end

    # Pops the last scene.
    def pop_scene
      @scenes.delete_at(-1)
    end

    # Registers a new scene with a given name. the block will be passed
    # to klass.new.
    #
    # @param [Symobl] name the name of the new scene
    # @param [Class] klass the class of the scene.
    def scene(name, klass = Scene, &block)
      @registred_scenes[name] = klass.new(&block)
    end

    # Runs the game until the last scene gets popped.
    def run
      until @scenes.empty?
        @runner = Ray::DSL::EventRunner.new

        @scenes.each do |scene|
          scene.game         = self
          scene.window       = @window
          scene.event_runner = @runner

          scene.register_events
        end

        scene = @scenes.last

        scene.need_render!
        scene.run
      end

      Ray.stop
    end
  end
end
