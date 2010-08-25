module Ray
  class Game
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

      # Biggest resolution is privilegied
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
