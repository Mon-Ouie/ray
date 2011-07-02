module Ray
  module GL
    class Vertex
      @vertex_classes = {0 => Ray::Vertex}

      # Creates a new Vertex class with a custom layout. Layout is an array of
      # arrays, where each row contains 3 elements:
      #
      # 1. Attribute name in Ruby
      # 2. Attribute name in GLSL shaders
      # 3. Attribute type, one of the following symbols: float, int, ubyte,
      #    bool, color, vector2, vector3.
      #
      # Getters and setters are created for all of the attributes.
      #
      # @param [Array] layout
      #
      # @example
      #   TestVertex = Ray::GL::Vertex.make [
      #     [:float,   "in_Float",    :float],
      #     [:int,     "in_Int",      :int],
      #     [:ubyte,   "in_Ubyte",    :ubyte],
      #     [:bool,    "in_Bool",     :bool],
      #     [:color,   "in_Color",    :color],
      #     [:vector2, "in_Vector2",  :vector2],
      #     [:vector3, "in_Vector3",  :vector3]
      #   ]
      def self.make(layout)
        layout.each do |_, _, type|
          unless TypeMap.has_key? type
            raise ArgumentError, "unknown type in a vertex: #{type.inspect}"
          end
        end
        vtype = make_type layout.map { |_, *rest| rest }

        @vertex_classes[vtype] = Class.new self do
          # Be *very* careful with those values.
          @vertex_type_id   = vtype
          @vertex_type_size = Vertex.size(vtype)

          class << self
            undef make
            undef make_type
            undef size
            undef offset_of
          end

          define_method :initialize do |*args|
            if args.size > layout.size
              raise ArgumentError, "wrong number of arguments: %d for %d",
                args.size, layout.size
            end

            layout.each_with_index do |(attr, _, type), i|
              send("#{attr}=", args[i] || default_for(type))
            end
          end

          layout.each_with_index do |(attr, _, type), i|
            offset = Vertex.offset_of(vtype, i)

            # Faster than define_method.
            module_eval(<<-eom, __FILE__, __LINE__)
              def #{attr}
                element(#{offset}, #{type.inspect})
              end

              def #{attr}=(val)
                set_element(#{offset}, #{type.inspect}, val)
              end
            eom

            alias_method "#{attr}?", attr if type == :bool
          end
        end
      end

      private
      def default_for(type)
        case type
        when :float, :int, :ubyte then 0
        when :bool                then true
        when :vector2             then Ray::Vector2[0, 0]
        when :vector3             then Ray::Vector3[0, 0, 0]
        when :color               then Ray::Color.white
        end
      end
    end
  end
end
