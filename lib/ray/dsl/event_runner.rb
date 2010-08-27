require 'thread'

module Ray
  module DSL
    # This class is the one that dispatches events in your program, the one that
    # makes everything work. You may want to create one by yourself if you don't
    # want to use the other classes that use it. You just have to call run every
    # time you need the events to be said.
    class EventRunner
      def initialize
        @handlers    = []
        @event_list  = []
        @next_events = []

        @mutex = Mutex.new
      end

      # Sends all the known events to our listeners.
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
