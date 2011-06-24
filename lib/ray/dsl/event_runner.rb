module Ray
  module DSL
    # This class is the one that dispatches events in your program, the one that
    # makes everything work. You may want to create one by yourself if you don't
    # want to use the other classes that use it. You just have to call run every
    # time you need the events to be processed.
    class EventRunner
      def initialize
        @handlers    = []
        @event_list  = []
        @next_events = []

        @event_groups = Hash.new { |h, k| h[k] = true }
        @event_groups[:default] = true
      end

      # Sends all the known events to our listeners.
      def run
        @event_list  = @next_events
        @next_events = []

        handlers = @handlers.select { |o| group_enabled?(o.group) }

        @event_list.each do |ev|
          handlers.select { |o| o.call(ev) if o.match?(ev) }
        end
      end

      def add_handler(type, group, args, block)
        @handlers << Ray::DSL::Handler.new(type, group, args, block)
      end

      def add_event(type, args)
        @next_events << Ray::DSL::Event.new(type, args)
      end

      # Disables an event group
      def disable_group(group)
        @event_groups[group] = false
      end

      # Enables an event group
      def enable_group(group)
        @event_groups[group] = true
      end

      # Removes all the handlers belonging to a given group
      def remove_group(name)
        @handlers.delete_if { |o| o.group == name }
      end

      # Removes all the registered handlers
      def clear
        @handlers.clear
      end

      # @return Whether an event group is enabled
      def group_enabled?(group)
        @event_groups[group]
      end
    end
  end
end
