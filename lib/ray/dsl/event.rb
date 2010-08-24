module Ray
  module DSL
    class Event
      @@descriptions = {}

      def self.add_description(name, types)
        @@descriptions[name] = types
      end

      def self.description_of(name)
        @@descriptions[name]
      end

      def initialize(type, args)
        @type = type
        @args = args

        if desc = @@descriptions[@type]
          desc.each_with_index do |type, i|
            begin
              @args[i] = Ray.convert(@args[i], type)
            rescue TypeError
              return
            end
          end
        end
      end

      attr_reader :type
      attr_reader :args
    end
  end

  def self.describe_event(name, *types)
    DSL::Event.add_description(name, types.map { |i| Ray.resolve_type(i) })
  end

  def self.description_for_event(name)
    DSL::Event.description_of(name)
  end
end
