module Ray
  module DSL
    # The events you can raise and listen to are instances of this class.
    # You should not manipulate them directly. In fact, they are created
    # and handled by the event runner, which happens to be greedy, and
    # therefore won't give an event to you. It will only give you its
    # arguments (you already know its type anyway.)
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

  # This method decribes an event, which means Ray will be able to understand
  # that, when you say "image.png", you actually meant Ray::Image.new("image.png").
  #
  # If Ray can't convert your arguments, it will just let you continue that way.
  def self.describe_event(name, *types)
    DSL::Event.add_description(name, types.map { |i| Ray.resolve_type(i) })
  end

  # @return [Array<Module>] The types of the arguments for a given event, in the
  #                         right order.
  def self.description_for_event(name)
    DSL::Event.description_of(name)
  end
end
