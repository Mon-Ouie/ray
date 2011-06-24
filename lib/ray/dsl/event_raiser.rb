module Ray
  module DSL
    # The module charged to raise your events, asking to an event runner to send
    # this event to the objects which registered to it.
    module EventRaiser
      # Raises a new event, with the given type and arguments.
      def raise_event(type, *args)
        return unless raiser_runner
        raiser_runner.add_event(type, args)
      end

      # @return [Ray::DSL::EventRunner] Event runner used to raise events.
      def raiser_runner
        @__raiser_runner
      end

      # Sets the event runner used to raise events.
      # @param [Ray::DSL::EventRunner] arg The new event runner.
      def raiser_runner=(arg)
        @__raiser_runner = arg
      end
    end
  end
end
