require File.expand_path(File.dirname(__FILE__) + '/../spec_helper')

if Ray.has_audio_support?
  describe Ray::Audio do
    it "should allow to set and get audio parameters" do
      Ray.init(:mono => true,
               :format => Ray::Audio::FORMAT_S8,
               :frequency => 44100)

      Ray::Audio.frequency.should == 44100
      Ray::Audio.should be_mono
      Ray::Audio.format.should == Ray::Audio::FORMAT_S8

      Ray.stop
    end

    it "should allow to change the volume" do
      Ray.init(:mono => true,
               :format => Ray::Audio::FORMAT_S8,
               :frequency => 44100)

      Ray::Audio.volume = 50
      Ray::Audio.volume.should be_close(50, 0.1)

      Ray.stop
    end
  end

  describe Ray::Sound do
    before :all do
      Ray.init
    end

    it "should allow to load sounds" do
      lambda {
        Ray::Sound.new(path_of("pop.wav"))
      }.should_not raise_exception
    end

    it "should allow to load sounds from IO objects" do
      lambda {
        open(path_of("pop.wav")) do |io|
          Ray::Sound.new(io)
        end
      }.should_not raise_exception
    end

    it "should allow to change the volume" do
      sound = Ray::Sound.new(path_of("pop.wav"))
      sound.volume = 50
      sound.volume.should be_close(50, 0.1)
    end

    it "should make Ray play" do
      sound = Ray::Sound.new(path_of("pop.wav"))

      lambda { sound.play(1) }.should change {
        Ray::Audio.playing?(1) && !Ray::Audio.paused?(1)
      }.from(false).to(true)

      Ray::Audio.stop(1)
    end

    it "can be paused" do
      sound = Ray::Sound.new(path_of("pop.wav"))
      sound.play(1)

      lambda { Ray::Audio.pause(1) }.should change {
        Ray::Audio.playing?(1) && !Ray::Audio.paused?(1)
      }.from(true).to(false)
    end

    it "can be resumed" do
      sound = Ray::Sound.new(path_of("pop.wav"))
      sound.play(1)
      Ray::Audio.pause(1)

      lambda { Ray::Audio.resume(1) }.should change {
        Ray::Audio.playing?(1) && !Ray::Audio.paused?(1)
      }.from(false).to(true)

      Ray::Audio.stop(1)
    end

    after :all do
      Ray.stop
    end
  end

  describe Ray::Music do
    before :all do
      Ray.init
    end

    it "should be able to create object from valid files" do
      lambda {
        Ray::Music.new(path_of("pop.wav"))
      }.should_not raise_exception
    end

    it "should be able to create object from IO objects" do
      lambda {
        open(path_of("pop.wav")) { |io| Ray::Music.new(io) }
      }.should_not raise_exception
    end

    it "should make Ray play" do
      music = Ray::Music.new(path_of("pop.wav"))
      music.play

      Ray::Audio.should be_playing
      Ray::Audio.stop
    end

    it "can be paused" do
      music = Ray::Music.new(path_of("pop.wav"))
      music.play
      Ray::Audio.pause

      Ray::Audio.should be_paused
    end

    it "can be resumed" do
      music = Ray::Music.new(path_of("pop.wav"))
      music.play

      Ray::Audio.pause
      Ray::Audio.resume

      Ray::Audio.should_not be_paused
      Ray::Audio.stop
    end

    it "can be stopped" do
      music = Ray::Music.new(path_of("pop.wav"))
      music.play

      Ray::Audio.stop
      Ray::Audio.should_not be_playing
    end

    after :all do
      Ray.stop
    end
  end
end
