module Ray
  class Shader
    class NoUniformError < StandardError
      def initialize(name)
        super "could not find uniform in shader: #{name}"
      end
    end

    # @param opts (see compile)
    def initialize(opts = nil)
      @locations = {}
      @images    = {}

      compile(opts) if opts
    end

    # @option opts [String, #read] :vertex A vertex shader (filename, or io)
    # @option opts [String, #read] :frag A fragment shader (filename, or io)
    # @option opts [String, #read] :geometry A geometry shader (filename, or io)
    #
    # Compiles the shader.
    def compile(opts)
      [:vertex, :frag, :geometry].each do |type|
        if opts[type]
          if opts[type].is_a? String
            opts[type] = File.read(opts[type])
          else
            opts[type] = opts[type].read
          end
        end
      end

      compile_vertex(opts[:vertex]) if opts[:vertex]
      compile_frag(opts[:frag]) if opts[:frag]
      compile_geometry(opts[:geometry]) if opts[:geometry]

      link

      @locations.clear
      @images.clear

      self
    end

    # @param [String, Symbol] attr Name of the parameter to set
    # @param
    def []=(attr, value)
      attr = attr.to_sym
      @locations[attr] ||= locate(attr)

      loc = @locations[attr]
      unless loc
        raise NoUniformError, attr
      end

      case value
      when Vector2
        set_vector2 loc, value
      when Vector3
        set_vector3 loc, value
      when Color
        set_color loc, value
      when Matrix
        set_matrix loc, value
      when true, false, nil
        set_bool loc, value
      when :current_texture
        set_current_texture loc
      when Image
        set_image loc, val
      when Array
        unless value.size.between? 1, 4
          raise "can't send a #{value.size}-sized vector"
        end

        set_array loc, value
      else # numeric-stuff
        set_numeric loc, value
      end
    end
  end
end
