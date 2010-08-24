module Ray
  module DSL
    # The module charged to raise your events, asking to an event runner to send
    # this event to the objects who said they were interested in it.
    module EventRaiser
      # Raises a new event, with the given type and arguments.
      def raise_event(type, *args)
        return unless raiser_runner
        raiser_runner.add_event(type, args)
      end

      def raiser_runner
        @__raiser_runner
      end

      def raiser_runner=(arg)
        @__raiser_runner = arg
      end
    end
  end
end
