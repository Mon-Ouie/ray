module Ray
  # Represents an audio channel, where sounds are played.
  class Channel
    # Creates a channel.
    # @param [Integer] id Identifer of the channel.
    def initialize(id)
      @id = id
    end

    # Plays a sound on the channel.
    # @param [Ray::Sound] sound The sound to be played
    # @param [Integer, :forever] How many times the sound should be played.
    def play(sound, times = 1)
      sound.play(@id, times)
    end

    # Stops playing on the channel, without being able to resume.
    def stop
      Ray::Audio.stop(@id)
    end

    # Pauses the channel.
    def pause
      Ray::Audio.pause(@id)
    end

    # Resumes from pause.
    def resume
      Ray::Audio.resume(@id)
    end

    # @return [true, false] True if the channel is paused.
    def paused?
      Ray::Audio.paused?(@id)
    end

    # @return [true, false] True if a sound is being played.
    def playing?
      Ray::Audio.playing?(@id)
    end
  end

  class Sound
    extend Ray::ResourceSet
    add_set(/^(.*)$/) { |filename| new(filename) }
  end

  class Music
    extend Ray::ResourceSet
    add_set(/^(.*)$/) { |filename| new(filename) }
  end
end
