module Ray
  # Effects are used to generate fragment shaders automatically. They represent
  # one of the transformations applied to the color.
  #
  # Effects must generate a structure to store arguments passed to the effect
  # (structure generation being done through a DSL), and a function to apply the
  # effect (a call to the #code method returns )
  class Effect
    # @overload effect_name(name)
    #   Sets the name of the effect
    #   @param [String] value
    #
    # @overload effect_name
    #   @return [String] Name of the effect
    def self.effect_name(value = nil)
      if value
        @name = value

        klass = self
        Ray::Helper.send :define_method, @name do |*args|
          klass.new(*args)
        end
      else
        @name
      end
    end

    # Adds an attribute to the shader. It is an element of the struct
    #
    # @param [String] name Name of the attribute
    # @param [String] type Type of the attribute. Arrays are written using this
    #  notation: type[count].
    def self.attribute(name, type)
      attributes[name] = type
    end

    # @return [Hash] All the attributes of the struct, matched with their type.
    def self.attributes
      @attributes ||= {}
    end

    # @return [String] Name of the effect
    def name
      self.class.effect_name
    end

    # @return [String] GLSL code of the struct.
    def struct
      str =  "struct ray_#{self.class.effect_name} {\n"
      str << "  bool enabled;\n"

      self.class.attributes.each do |name, type|
        if type =~ /\A(\w+)\[(\d+)\]\z/
          str << "  #$1 #{name}[#$2];\n"
        else
          str << "  #{type} #{name};\n"
        end
      end

      str << "};\n"
    end

    # @abstract
    #
    # @return [String] Code that must be written *before* the definiton of the
    #   structure. Just returns an empty string by default.
    def header
      ""
    end

    # @abstract
    #
    # @return [String]
    #   Code of the function. This must contain the header of the function.
    #   The name of the function is the name of the effect prepended by
    #   "do_". It is passed a ray_#{effect_name} structure and the color, and is
    #   expected to return the changed color.
    #
    # @example
    #   def code
    #     return %{
    #       vec4 do_some_effect(ray_some_effect arg, vec4 color) {
    #         /* Do magic here. */
    #       }
    #     }
    #   end
    def code
      raise NotImplementedError
    end

    # @abstract
    #
    # @return [Hash] Default value for each attribute.
    def defaults
      {}
    end

    # Apply the defaults to a shader.
    # @param [Ray::Shader] shader
    def apply_defaults(shader)
      effect_name = self.class.effect_name

      shader["#{effect_name}.enabled"] = true

      defaults.each do |name, value|
        shader["#{effect_name}.#{name}"] = value
      end
    end
  end
end

require 'ray/effect/generator'

require 'ray/effect/grayscale'
