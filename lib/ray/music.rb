module Ray
  class Music
    def initialize(filename = nil)
      open(filename) if filename
    end

    alias time= seek

    alias position  pos
    alias position= pos=
  end
end
