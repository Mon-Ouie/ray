require File.expand_path(File.dirname(__FILE__) + '/../spec_helper')

describe Ray::Image do
  before :all do
    Ray.init
    @win = Ray.create_window(:w => 100, :h => 100)
  end

  it "should raise a type error when blitting on something that isn't an image" do
    img = Ray::Image.new(:w => 50, :h => 50)
    lambda {
      img.blit(:on => Ray::Color.new(10, 20, 30))
    }.should raise_exception(TypeError)
  end

  describe "#initialize" do
    it "should raise a type error if the argument isn't a hash or a string " do
      lambda {
        Ray::Image.new(3)
      }.should raise_exception(TypeError)
    end

    it "should not raise an error when loading an existing image" do
      lambda {
        Ray::Image.new(path_of("aqua.bmp"))
      }.should_not raise_exception
    end

    it "should raise an error when loading an unexisting file " do
      lambda {
        Ray::Image.new("does_not_exist.bmp")
      }.should raise_exception
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

      it "should load images even if the extension is incorrect" do
        lambda {
          Ray::Image.new(path_of("not_a_jpeg.jpeg"))
        }.should_not raise_exception
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

  describe "#==" do
    it "should be true if the parameter uses the same surface" do
      @win.should == Ray.screen

      first = Ray::Image.new(:w => 10, :h => 10).fill(Ray::Color.red)
      sec = Ray::Image.new(:w => 10, :h => 10).fill(Ray::Color.red)

      first.should_not == sec
    end

    it "should not raise an exception for non-images" do
      lambda {
        @win == 3
      }.should_not raise_exception
    end
  end

  describe "#dup" do
    it "should create a new surface" do
      first = Ray::Image.new(:w => 10, :h => 10).fill(Ray::Color.red)
      sec = first.dup

      first.should_not == sec
    end
  end

  describe "#[]" do
    it "should return nil for pixels outside the image" do
      Ray::Image.new(:w => 10, :h => 10)[10, 10].should be_nil
    end

    it "should return a color for pixels inside the image" do
      Ray::Image.new(:w => 10, :h => 10)[5, 5].should be_a(Ray::Color)
    end
  end

  describe "#[]=" do
    it "should change the color of the pixel" do
      img = Ray::Image.new(:w => 10, :h => 10)
      img.lock { img[5, 5] = Ray::Color.new(10, 15, 20) }

      col = img[5, 5]
      col.r.should == 10
      col.g.should == 15
      col.b.should == 20
    end

    it "should raise an error for pixels outside the image" do
      img = Ray::Image.new(:w => 10, :h => 10)
      lambda {
        img.lock { img[11, 11] = Ray::Color.new(10, 15, 20) }
      }.should raise_exception
    end
  end

  describe "#clip" do
    it "should return the clipping rect if there are no arguments" do
      img = Ray::Image.new(:w => 10, :h => 10)
      img.clip.should == Ray::Rect.new(0, 0, 10, 10)
    end

    it "should change the clip rect when called with a rect" do
      img = Ray::Image.new(:w => 10, :h => 10)
      img.clip([5, 5, 3, 3])
      img.clip.should == Ray::Rect.new(5, 5, 3, 3)
    end

    it "should return the new rect" do
      img = Ray::Image.new(:w => 10, :h => 10)
      img.clip([5, 5, 3, 3]).should == Ray::Rect.new(5, 5, 3, 3)
    end

    it "should change the clipping rect only within a block" do
      img = Ray::Image.new(:w => 10, :h => 10)
      img.clip([5, 5, 3, 3]) do
        img.clip.should == Ray::Rect.new(5, 5, 3, 3)
      end

      img.clip.should == Ray::Rect.new(0, 0, 10, 10)
    end
  end

  after :all do
    Ray.stop
  end
end
