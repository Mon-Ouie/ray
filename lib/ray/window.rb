module Ray
  class Window
    # @yieldparam [Ray::Event] event Each event to be processed
    def each_event
      return Enumerator.new(self, :each_event) unless block_given?

      ev = Ray::Event.new

      until poll_event(ev).type == Ray::Event::TypeNone
        yield ev
      end
    end
  end
end
