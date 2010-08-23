module Ray
  module DSL
    class Handler
      def initialize(type, args, block)
        @type, @args, @block = type, args, block
      end

      def match?(event)
        return false unless event.type == @type
        return match_args?(event.args) unless @args.empty?
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

      private
      def match_args?(args)
        return false if @args.size != args.size

        @args.each_with_index do |elem, i|
          other = args[i]

          case elem
          when Ray::DSL::Matcher
            return false unless elem.match?(other)
          when Regexp
            if other.is_a? Regexp
              return false unless elem == other
            elsif other.is_a? String
              return false unless elem =~ other
            else
              return false
            end
          else
            return false unless elem == other
          end
        end

        return true
      end
    end
  end
end
