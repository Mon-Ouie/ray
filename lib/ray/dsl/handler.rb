module Ray
  module DSL
    class Handler
      def initialize(type, args, block)
        @type, @args, @block = type, args, block
      end

      def match?(event)
        return false unless event.type == @type
        return @args == event.args unless @args.empty?
        true
      end

      def call(event)
        if @block.arity == 0
          @block.call
        else
          @block.call(*event.args)
        end
      end

      attr_reader :type
      attr_reader :args
    end
  end
end
