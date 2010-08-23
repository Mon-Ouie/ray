module Ray
  module DSL
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
