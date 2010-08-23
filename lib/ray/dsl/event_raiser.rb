module Ray
  module DSL
    module EventRaiser
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
