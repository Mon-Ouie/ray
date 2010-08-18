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
  end

  after :each do
    Ray.stop
  end
end
