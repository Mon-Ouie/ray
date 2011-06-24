module Ray
  class Music
    def initialize(filename = nil)
      open(filename) if filename
    end

    alias time= seek
  end
end
