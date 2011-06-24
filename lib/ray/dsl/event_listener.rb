module Ray
  module DSL
    # The module that allows you to do something when something else happened.
    module EventListener
      # Registers a block to listen to an event.
      #
      # Arguments are compared with the ones of the regex using === and,
      # if it failed, ==.
      #
      # @param [Symbol] event Name of the event to listen to.
      # @param [DSL::Matcher, Regexp, Object] args List of arguments that should
      #   match the event's (if empty, the block will be called anyway).
      def on(event, *args, &block)
        return unless listener_runner
        listener_runner.add_handler(event, current_event_group, args, block)
      end

      # @overload add_hook(event, ..., callable)
      #   Same as #on, but the last argument is an object which responds to
      #   #to_proc.
      #
      #   @example
      #     add_hook :quit, method(:exit!)
      def add_hook(event, *args)
        return unless listener_runner
        listener_runner.add_handler(event, current_event_group, args[0...-1],
                                    args.last)
      end

      # Sets the current event group temporarily.
      # @param name Name of the event group.
      # @yield Runs a block where registered event will be registred to the
      #   event group.
      def event_group(name)
        old_group = current_event_group
        self.current_event_group = name

        begin
          yield name
        ensure
          self.current_event_group = old_group
        end
      end

      def current_event_group
        @__listener_event_group ||= :default
      end

      def current_event_group=(val)
        @__listener_event_group = val
      end

      # @return [Ray::DSL::EventRunner] Object where matchers will be registred.
      def listener_runner
        @__listener_runner if defined?(@__listener_runner)
      end

      # Sets the event runner which will be used for listening.
      # @param [Ray::DSL::EventRunner] arg The new even runner.
      def listener_runner=(arg)
        @__listener_runner = arg
      end
    end
  end
end
