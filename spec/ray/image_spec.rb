describe Ray::Image do
  before :each do
    Ray.init
    @win = Ray.create_window(:w => 100, :h => 100)
  end

  describe "#blit" do
    context "when trying to blit on a non-surface" do
      it "should raise a type error" do
        img = Ray::Image.new(:w => 50, :h => 50)
        lambda {
          img.blit(:on => Ray::Color.new(10, 20, 30))
        }.should raise_exception(TypeError)
      end
    end

    context "when trying to use a non-rect as a position" do
      it "should raise a type error" do
        img = Ray::Image.new(:w => 50, :h => 50)
        lambda {
          img.blit(:from => Ray::Color.new(10, 20, 30), :on => @win)
        }.should raise_exception(TypeError)

        lambda {
          img.blit(:on => @win, :at => Ray::Color.new(10, 20, 30))
        }.should raise_exception(TypeError)
      end
    end
  end

  describe "#initialize" do
    context "when the argument isn't a hash or a string" do
      it "should raise a type error" do
        lambda {
          Ray::Image.new(3)
        }.should raise_exception(TypeError)
      end
    end

    context "when loading an existing file" do
      it "should not raise an error" do
        lambda {
          Ray::Image.new(path_of("aqua.bmp"))
        }.should_not raise_exception
      end
    end

    context "when loading an unexising file" do
      it "should raise a runtime error" do
        lambda {
          Ray::Image.new("does_not_exist.bmp")
        }.should raise_exception
      end
    end

    it "should be able to load a file from an IO" do
      File.open(path_of("aqua.bmp")) do |io|
        lambda {
          img = Ray::Image.new(io)
        }.should_not raise_exception
      end
    end

    if Ray.has_image_support?
      it "should be able to load other kind of images" do
        lambda {
          Ray::Image.new(path_of("aqua.png"))
        }.should_not raise_exception
      end

      it "should be able to load other kind of images from an IO" do
        File.open(path_of("aqua.png")) do |io|
          lambda {
            Ray::Image.new(io)
          }.should_not raise_exception
        end
      end

      context "if the extension does not match the file type" do
        it "should still load it correctly" do
          lambda {
            Ray::Image.new(path_of("not_a_jpeg.jpeg"))
          }.should_not raise_exception
        end
      end
    end
  end

  describe ".[]" do
    it "should return a new image" do
      Ray::Image[path_of("aqua.bmp")].should be_a(Ray::Image)
    end

    it "should always return the same image" do
      obj = Ray::Image[path_of("aqua2.bmp")]
      obj.object_id.should == Ray::Image[path_of("aqua2.bmp")].object_id
    end
  end

  after :each do
    Ray.stop
  end
end
