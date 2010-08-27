module Ray
  module DSL
    # Used internally to store event when raise_event is caled.
    class Event
      def initialize(type, args)
        @type = type
        @args = args
      end

      attr_reader :type
      attr_reader :args
    end
  end
end
