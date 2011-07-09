module Ray
  # Class used by games to manage their scene list.
  # It needs a game object to find a scene from its name.
  class SceneList
    include Enumerable

    # @param [Ray::Game] game The game which will be used to find scenes.
    def initialize(game)
      @game  = game
      @scenes     = []
      @scene_args = []
    end

    # @return [true, false] True if the scene list contains no scene
    def empty?
      @scenes.empty?
    end

    # @return [Ray::Scene, nil] The current scene
    def current
      @scenes.last
    end

    # Rune the current scene
    def run_current
      scene = @scenes.last

      scene.scene_arguments = @scene_args.last

      scene.setup(*@scene_args.last)
      scene.register_events
      scene.run
    end

    # Exits the current scene
    def exit_current
      return if empty?
      current.exit
    end

    # Pops the last scene
    def pop
      @scenes.pop
      @scene_args.pop
    end

    # Clears the scene list
    def clear
      @scenes.clear
      @scene_args.clear
    end

    # @param [Symbol] scene Name of the scene
    def push(scene_name, *args)
      scene = @game.registered_scene(scene_name)
      raise ArgumentError, "Unknown scene #{scene_name}" unless scene

      @scenes     << scene
      @scene_args << args

      self
    end

    alias :<< :push

    def each(&block)
      @scenes.each(&block)
    end

    def inspect
      "#{self.class}#{@scenes.inspect}"
    end
  end
end
