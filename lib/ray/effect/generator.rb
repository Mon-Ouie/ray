module Ray
  class Effect
    class Generator
      include Enumerable

      # @param [Integer] version GLSL version to use
      # @yield Yields itself if a block is given
      def initialize(version = 110)
        @effects = []
        @version = version

        if version >= 130
          @input = <<-input
in vec4 var_Color;
in vec2 var_TexCoord;
input
        else
          @input = <<-input
varying vec4 var_Color;
varying vec2 var_TexCoord;
input
        end

        @uniforms = <<-uniforms
uniform sampler2D in_Texture;
uniform bool in_TextureEnabled;
uniforms

        @color = "color"

        @default = <<-default
  /* Apply default value */
  vec4 color;
  if (in_TextureEnabled)
    color = texture2D(in_Texture, var_TexCoord) * var_Color;
  else
    color = var_Color;
default

        yield self if block_given?
      end

      # @param [Array<Ray:::Effect>] effects effects to add to the generator
      def push(*effects)
        @effects.concat effects
        self
      end

      alias << push

      def each(&block)
        @effects.each(&block)
      end

      # @return [Array<Ray::Effect>] All of the effects used by the generator
      attr_reader :effects
      alias to_a effects

      # @return [Integer] GLSL version number
      attr_reader :version

      # @return [String] Code defining GLSL input (with varying or in, depending
      #   on the GLSL version).
      attr_accessor :input

      # @return [String] Code defining uniforms
      attr_accessor :uniforms

      # @return [String] Name of the variable containing the
      #   color once the default code is run. "color" by default.
      attr_accessor :color

      # @return [String] Code to ste the default color. It should apply
      #   texturing, for example.
      attr_accessor :default

      # @return [String] code of the pixel shader
      def code
        str  = "#version #@version\n"
        str << "\n"

        str << input    << "\n"
        str << uniforms << "\n"


        str << "out vec4 out_FragColor;\n" if version >= 130
        str << "\n"

        str << "/* Headers */\n"
        each do |effect|
          str << effect.header << "\n"
        end
        str << "\n"

        str << "/* Structs */\n\n"
        each do |effect|
          str << effect.struct << "\n"
        end

        str << "/* Effects parameters */\n"
        each do |effect|
          str << "uniform ray_#{effect.name} #{effect.name};\n"
        end
        str << "\n"

        str << "/* Functions */\n"
        each do |effect|
          str << effect.code << "\n"
        end

        str << "void main() {\n"
        str << default << "\n"

        each do |effect|
          str << "  if (#{effect.name}.enabled)\n"
          str << "    #@color = do_#{effect.name}(#{effect.name}, #@color);\n"
          str << "\n"
        end

        if version >= 130
          str << "  out_FragColor = #@color;\n"
        else
          str << "  gl_FragColor = #@color;\n"
        end

        str << "}\n"
      end

      def build(shader = Ray::Shader.new)
        shader.compile :frag => StringIO.new(code)
        apply_defaults shader
      end

      def apply_defaults(shader)
        each { |effect| effect.apply_defaults(shader) }
      end
    end
  end
end
