module Ray
  class Music
    def initialize(filename = nil)
      open(filename) if filename
    end

    def pretty_print(q)
      super q, ["time", "duration", "looping?"]
    end

    alias time= seek

    alias position  pos
    alias position= pos=
  end
end
