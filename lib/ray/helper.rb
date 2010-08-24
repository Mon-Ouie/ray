module Ray
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
