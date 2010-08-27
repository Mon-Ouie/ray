module Ray
  module DSL
    # Used internally to call blocks registred with Ray::DSL::Listener#on.
    class Handler
      def initialize(type, args, block)
        @type, @args, @block = type, args, block
      end

      def match?(event)
        return false unless event.type == @type
        return match_args?(event.args) unless @args.empty?
        true
      end

      alias :=== :match?

      def call(event)
        if @block.arity == 0
          @block.call
        else
          @block.call(*event.args)
        end
      end

      attr_reader :type
      attr_reader :args

      private
      def match_args?(args)
        return false if @args.size > args.size

        @args.each_with_index do |elem, i|
          other = args[i]
          return false unless (elem === args[i]) || (elem == args[i])
        end

        return true
      end
    end
  end
end
