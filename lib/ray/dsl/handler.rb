module Ray
  module DSL
    # Used internally to call blocks registred with Ray::DSL::Listener#on.
    class Handler
      def initialize(type, args, block)
        @type, @args, @block = type, args, block

        if desc = Ray.description_for_event(@type)
          desc.each_with_index do |type, i|
            next if @args[i].is_a? Matcher
            next if @args[i].is_a? Regexp

            begin
              @args[i] = Ray.convert(@args[i], type)
            rescue TypeError
              return
            end
          end
        end
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
