module Ray
  module DSL
    # The module that allows you to do something when something else happened.
    module Listener
      # The args part of this method may help you to say when you want your
      # block to be called. Each argument applies to one argument of the
      # event, and they must all match or your block won't get called.
      #
      # It will use == to see if two objects match, except in two cases: if you
      # pass a Matcher, it will use match?, and if you pass a regex it will try
      # to use =~.
      #
      # Your block will be called with the same arguments as the event, just in
      # case you'd need them.
      #
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
