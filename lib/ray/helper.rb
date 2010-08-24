module Ray
  # This module includes the methods you may want to use in your code.
  # It allows you to play with events (listen to them, raise them) and
  # includes the matchers you described, which you may want to use when
  # you want to listen to the events matching a condition.
  module Helper
    include Ray::DSL::EventRaiser
    include Ray::DSL::Listener

    include Ray::Matchers

    def event_runner=(value)
      self.listener_runner = value
      self.raiser_runner   = value
    end
  end
end
