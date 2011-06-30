module Ray
  class Window
    # @param title (see #open)
    # @param size  (see #open)
    # @param opts  (see #open)
    def initialize(title = nil, size = [640, 480], opts = {})
      open(title, size, opts) if title
    end

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
