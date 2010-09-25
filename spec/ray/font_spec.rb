require File.expand_path(File.dirname(__FILE__) + '/../spec_helper')

if Ray.has_font_support?
  describe Ray::Font do
    before :all do
      Ray.init
    end

    it "should be able to load fonts from a file" do
      lambda {
        Ray::Font.new(path_of("VeraMono.ttf"), 12)
      }.should_not raise_exception
    end

    it "should be able to load fonts from IO objects" do
      lambda {
        open(path_of("VeraMono.ttf")) { |io| Ray::Font.new(io, 12) }
      }.should_not raise_exception
    end

    it "should create normal font" do
      Ray::Font.new(path_of("VeraMono.ttf"), 12).should be_normal
    end

    it "should allow to change its style" do
      font = Ray::Font.new(path_of("VeraMono.ttf"), 12)
      font.style = Ray::Font::STYLE_BOLD | Ray::Font::STYLE_ITALIC

      font.should be_italic
      font.should be_bold
    end

    describe "#draw" do
      before :all do
        @win  = Ray.create_window(:w => 100, :h => 100)
        @font = Ray::Font.new(path_of("VeraMono.ttf"), 12)
      end

      it "should return an image when we don't say where to draw" do
        @font.draw("Something").should be_a(Ray::Image)
      end

      it "should return the surface it drew on if it did" do
        @font.draw("Something", :on => @win, :at => [0, 0]).should == @win
      end
    end

    after :all do
      Ray.stop
    end
  end

  describe "String#draw" do
    before :all do
      Ray.init

      @win  = Ray.create_window(:w => 100, :h => 100)
      @font = Ray::Font.new(path_of("VeraMono.ttf"), 12)
    end

    it "should ask for the width unless :on is specified" do
      lambda {
        "Hello world!".draw(:font => @font)
      }.should raise_exception
    end

    it "should not ask for the width if :on is specified" do
      lambda {
        "Hello world!".draw(:font => @font, :on => @win)
      }.should_not raise_exception
    end

    it "should use the specified width unless :on is specified" do
      img = "Hello world!".draw(:font => @font, :w => 40)
      img.w.should == 40
    end

    it "should require the font to be specified" do
      lambda {
        "Hello world".draw(:on => @win)
      }.should raise_exception
    end

    it "should draw on multiple lines" do
      img = "Hello\nworld".draw(:font => @font, :w => 100)
      img.height.should == @font.line_skip * 2
    end

    it "should allow to specify an explicit size" do
      img = "Hello\nworld".draw(:font => @font, :h => 100, :w => 50)
      img.width.should == 50
      img.height.should == 100
    end

    after :all do
      Ray.stop
    end
  end
end
