module Ray
  class Sound
    def initialize(arg = nil)
      case arg
      when String      then self.buffer = SoundBufferSet[arg]
      when SoundBuffer then self.buffer = arg
      when nil         then return
      else
        self.buffer = SoundBuffer.new(arg)
      end
    end

    alias time= seek
  end
end
