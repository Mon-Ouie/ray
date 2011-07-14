module Ray
  # A module containing classes you may play with, altough you'll usually not
  # need to, the other classes and methods written with them being what
  # you should use instead.
  module DSL; end

  class NoRunnerError < StandardError
    def initialize(o)
      super "#{o.inspect} has no runner object"
    end
  end
end

require 'ray/dsl/matcher'

require 'ray/dsl/event'
require 'ray/dsl/handler'
require 'ray/dsl/event_runner'

require 'ray/dsl/event_listener'
require 'ray/dsl/event_raiser'

require 'ray/dsl/event_translator'
