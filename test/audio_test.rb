require File.expand_path(File.dirname(__FILE__)) + '/helpers.rb'

context Ray::Audio do
  setup { Ray::Audio }

  asserts(:volume).equals(100)

  asserts(:pos).equals(Ray::Vector3[0, 0, 0])
  asserts(:direction).equals(Ray::Vector3[0, 0, -1])

  context "after changing the volume" do
    hookup { topic.volume = 50 }
    asserts(:volume).equals(50)
  end

  context "after changing the position" do
    hookup { topic.pos = [10, -5, 3] }
    asserts(:pos).equals(Ray::Vector3[10, -5, 3])
  end

  context "after changing the direction" do
    hookup { topic.direction = [10, -5, 3] }
    asserts(:direction).equals(Ray::Vector3[10, -5, 3])
  end
end

context Ray::SoundBuffer do
  setup { Ray::SoundBuffer }

  denies(:new, path_of("pop.wav")).raises_kind_of Exception

  denies("new from IO") {
    open(path_of("pop.wav")) { |io| topic.new(io) }
  }.raises_kind_of Exception
  denies(:new, File.open(path_of("pop.wav"))).raises_kind_of Exception

  asserts(:new, path_of("doesnt_exist.wav")).raises_kind_of Exception
  asserts(:new, path_of("sprite.png")).raises_kind_of Exception
  asserts("new from invalid IO") {
    open(path_of("aqua.png")) { |io| new(io) }
  }.raises_kind_of Exception
end

context "an audio source" do
  setup { Ray::Sound.new path_of("pop.wav") }

  asserts(:pos).equals Ray::Vector3[0, 0, 0]
  asserts(:pitch).equals 1
  denies(:relative?)
  asserts(:min_distance).equals 1
  asserts(:attenuation).equals 1
  asserts(:volume).equals 100

  context "after changing position" do
    hookup { topic.pos = [10, 20, 30] }
    asserts(:pos).equals Ray::Vector3[10, 20, 30]
  end

  context "after changing pitch" do
    hookup { topic.pitch = 30 }
    asserts(:pitch).equals 30
  end

  context "after changing relativity" do
    hookup { topic.relative = true }
    asserts(:relative?)
  end

  context "after changing minimal distance" do
    hookup { topic.min_distance = 0.5 }
    asserts(:min_distance).equals 0.5
  end

  context "after changing attenuation" do
    hookup { topic.attenuation = 15 }
    asserts(:attenuation).equals 15
  end

  context "after changing volume" do
    hookup { topic.volume = 40 }
    asserts(:volume).equals 40
  end
end

context "a sound" do
  setup do
    sound = Ray::Sound.new path_of("pop.wav")
    sound.volume = 0
    sound
  end

  asserts(:buffer).equals Ray::SoundBuffer[path_of("pop.wav")]
  asserts(:duration).equals { topic.buffer.duration }

  asserts(:time).equals 0

  denies(:looping?)

  context "after enabling looping" do
    hookup { topic.looping = true }
    asserts(:looping?)
  end

  context "after starting playback" do
    hookup { topic.play }
    asserts(:status).equals :playing

    context "and pausing it" do
      hookup { topic.pause }
      asserts(:status).equals :paused
    end

    context "and stopping it" do
      hookup { topic.stop }
      asserts(:status).equals :stopped
    end
  end
end

context "a music" do
  setup do
    sound = Ray::Music.new path_of("pop.wav")
    sound.volume = 0
    sound
  end

  asserts(:duration).equals Ray::SoundBuffer[path_of("pop.wav")].duration

  asserts(:time).equals 0

  denies(:looping?)

  context "after enabling looping" do
    hookup { topic.looping = true }
    asserts(:looping?)
  end

  context "after starting playback" do
    hookup { topic.play }
    asserts(:status).equals :playing

    context "and pausing it" do
      hookup { topic.pause }
      asserts(:status).equals :paused
    end

    context "and stopping it" do
      hookup { topic.stop }
      asserts(:status).equals :stopped
    end
  end
end

run_tests if __FILE__ == $0
