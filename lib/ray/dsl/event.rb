module Ray
  module DSL
    # Used internally to store event when raise_event is called.
    Event = Struct.new(:type, :args)
  end
end
