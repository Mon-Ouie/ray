module Ray
  module GL
    class Vertex
      @vertex_classes = {0 => Ray::Vertex}

      class Instance
        @instance_classes = {}

        class << self
          attr_reader :instance_classes
        end

        def self.layout
          @vertex_instance_layout
        end

        def to_s
          pairs = []
          self.class.layout.each do |key, _, _, per_instance|
            pairs << "#{key}=#{send(key)}" if per_instance
          end

          "#<#{self.class} #{pairs.join " "}>"
        end
      end

      # Creates a new Vertex class with a custom layout. Layout is an array of
      # arrays, where each row contains 3 or 4 elements:
      #
      # 1. Attribute name in Ruby
      # 2. Attribute name in GLSL shaders
      # 3. Attribute type, one of the following symbols: float, int, ubyte,
      #    bool, color, vector2, vector3.
      # 4. Whether the attribute is only accessibel on a per-instance basis (as
      #    opposed to per-vertex, which is the default)
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
      #     [:vector3, "in_Vector3",  :vector3],
      #
      #     # per-instance data
      #     [:instance_color, "magic", :color, true]
      #   ]
      def self.make(layout)
        layout.each do |_, _, type, _|
          unless TypeMap.has_key? type
            raise ArgumentError, "unknown type in a vertex: #{type.inspect}"
          end
        end

        vtype = make_type layout.map { |_, *rest| rest }

        @vertex_classes[vtype] = Class.new self do
          # Be *very* careful with those values.
          @vertex_type_id     = vtype
          @vertex_type_size   = Vertex.size(vtype)
          @vertex_type_layout = layout

          if layout.any? { |_, _, _, per_instance| per_instance }
            const_set :Instance, Class.new(Ray::GL::Vertex::Instance) {
              @vertex_instance_type_id = vtype
              @vertex_instance_size    = Vertex.instance_size(vtype)
              @vertex_instance_layout  = layout

              class << self
                undef instance_classes
              end

              Vertex.define_layout self, layout, vtype, true
            }
          end

          class << self
            undef make
            undef make_type
            undef size
            undef offset_of
          end

          Vertex.define_layout self, layout, vtype, false
        end

        if @vertex_classes[vtype].const_defined? :Instance
          Instance.instance_classes[vtype] = @vertex_classes[vtype]::Instance
        end

        @vertex_classes[vtype]
      end

      def self.layout
        @vertex_type_layout
      end

      def self.default_for(type)
        case type
        when :float, :int, :ubyte then 0
        when :bool                then true
        when :vector2             then Ray::Vector2[0, 0]
        when :vector3             then Ray::Vector3[0, 0, 0]
        when :color               then Ray::Color.white
        end
      end

      def self.define_layout(on, layout, vtype, instance = false)
        argument_layout = layout.reject do |_, _, _, per_instance|
          per_instance ^ instance
        end

        on.class_eval do
          define_method :initialize do |*args|
            if args.size > layout.size
              msg = "wrong number of arguments: #{args.size} for" <<
                " #{layout.size}"
              raise ArgumentError, msg
            end

            argument_layout.each_with_index do |(attr, _, type, _), i|
              send("#{attr}=", args[i] || Vertex.default_for(type))
            end
          end

          layout.each_with_index do |(attr, _, type, per_instance), i|
            next if per_instance ^ instance

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

      def to_s
        pairs = []
        self.class.layout.each do |key, _, _, per_instance|
          pairs << "#{key}=#{send(key)}" unless per_instance
        end

        "#<#{self.class} #{pairs.join " "}>"
      end
    end
  end
end
