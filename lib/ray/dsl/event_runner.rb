module Ray
  module DSL
    class EventRunner
      def initialize
        @handlers    = []
        @event_list  = []
        @next_events = []

        @mutex = Mutex.new
      end

      def run
        @mutex.synchronize do
          @event_list  = @next_events
          @next_events = []
        end

        @event_list.each do |ev|
          @handlers.select { |i| i.match? ev }.each do |handler|
            handler.call(ev)
          end
        end
      end

      def add_handler(type, args, block)
        @handlers << Ray::DSL::Handler.new(type, args, block)
      end

      def add_event(type, args)
        @mutex.synchronize { @next_events << Ray::DSL::Event.new(type, args) }
      end
    end
  end
end
