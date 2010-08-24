module Ray
  # A module containing classes you may play with, altough you'll usually not
  # need to, the other classes and methods written with them being what
  # you should use instead.
  module DSL; end
end

require 'ray/dsl/type'
require 'ray/dsl/matcher'
require 'ray/dsl/converter'

require 'ray/dsl/event'
require 'ray/dsl/handler'
require 'ray/dsl/event_runner'

require 'ray/dsl/listener'
require 'ray/dsl/event_raiser'
