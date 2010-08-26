module Ray
  class Scene
    include Ray::Helper

    class << self
      # If you want to subclass Scene, this is the method you need to call.
      # It will return a subclass, responding to bind, which allows you to
      # register it.
      #
      # @example
      #   Klass = Scene.create(:foo) do ... end
      #   Klass.bind(game)
      #   game.push_scene(:foo)
      def create(scene_name, &block)
        klass = Class.new(self)
        klass.instance_variable_set("@block", block)

        (class << klass; self; end).class_eval do
          define_method(:bind) do |game|
            game.scene(scene_name, self, &@block)
          end

          define_method(:inspect) { "Scene:#{scene_name}" }
          define_method(:block) { @block }
        end

        klass.class_eval do
          define_method(:initialize) do
            super(&self.class.block)
          end
        end

        return klass
      end
    end

    # Creates a new scene. block will be instance evaluated when
    # this scene becomes the current one.
    def initialize(&block)
      @exit = false
      @block = block
    end

    def register_events
      instance_eval(&@block)
    end

    # Runs until you exit the scene.
    # This will also raise events if the mouse moves, ... allowing you
    # to directly listen to a such event.
    def run
      until @exit
        DSL::EventTranslator.translate_event(Ray::Event.new).each do |args|
          raise_event(*args)
        end

        @always.call if @always

        listener_runner.run

        if @need_render
          @need_render = false

          @render.call(@window)
          @window.flip
        end
      end
    end

    # Exits the scene, but does not pops the scene (the next scene
    # will be the same one)
    def exit
      @exit = true
    end

    # Exits the scene and pops it.
    def exit!
      exit
      game.pop_scene
    end

    # Register a block to be excuted as often as possible.
    def always(&block)
      @always = block
    end

    # Marks the scene should be redrawn.
    def need_render!
      @need_render = true
    end

    # Registers the block to draw the scene.
    #
    # @yield [window] Block to render this scene.
    # @yieldparam [Ray::Image] window The window you should draw on
    def render(&block)
      @render = block
    end

    attr_accessor :game
    attr_accessor :window
  end
end
