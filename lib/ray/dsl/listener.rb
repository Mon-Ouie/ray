module Ray
  module DSL
    module Listener
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
