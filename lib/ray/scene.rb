module Ray
  class Scene
    include Ray::Helper

    class << self
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

    def initialize(&block)
      @exit = false
      @block = block
    end

    def register_events
      instance_eval(&@block)
    end

    def run
      until @exit
        ev = DSL::EventTranslator.translate_event(Ray::Event.new)
        raise_event(*ev) if ev

        @always.call if @always

        listener_runner.run

        if @need_render
          @need_render = false

          @render.call(@window)
          @window.flip
        end
      end
    end

    def exit
      @exit = true
    end

    def exit!
      exit
      game.pop_scene
    end

    def always(&block)
      @always = block
    end

    def need_render!
      @need_render = true
    end

    def render(&block)
      @render = block
    end

    attr_accessor :game
    attr_accessor :window
  end
end
