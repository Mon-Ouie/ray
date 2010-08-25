module Ray
  class Game
    def initialize(hash = {}, &block)
      @registred_scenes = {}
      @scenes = []

      # Not implemented
    end

    def push_scene(scene_name)
      scene = @registred_scenes[scene_name]
      raise ArgumentError, "Unknown scene #{scene_name}" unless scene

      @scenes << scene
    end

    def pop_scene
      @scenes.delete_at(-1)
    end

    def scene(name, klass = Scene, &block)
      @registred_scenes[name] = klass.new(&block)
    end

    def run
      until @scenes.empty?
        @runner = Ray::DSL::EventRunner.new

        @scenes.each do |scene|
          scene.game         = self
          scene.event_runner = @runner

          scene.register_events
        end

        @scenes.last.run
      end
    end
  end
end
