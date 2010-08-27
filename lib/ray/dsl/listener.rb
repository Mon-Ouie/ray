module Ray
  module DSL
    # The module that allows you to do something when something else happened.
    module Listener
      # Registers a block to listen to an event.
      #
      # Arguments are compared with the ones of the regex using === and,
      # if it failed, ==.
      #
      # @param [Symbol] event Name of the event to listen to
      # @param [DSL::Matcher, Regexp, Object] args List of arguments that should
      #                                            match the event's (if empty,
      #                                            the block will be called anyway)
      def on(event, *args, &block)
        return unless listener_runner
        listener_runner.add_handler(event, args, block)
      end

      def listener_runner
        @__listener_runner
      end

      def listener_runner=(arg)
        @__listener_runner = arg
      end
    end
  end
end
