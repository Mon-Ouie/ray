require File.expand_path(File.dirname(__FILE__) + '/../spec_helper')

describe Ray::Sprite do
  before :all do
    Ray.init
    @win = Ray.create_window(:w => 100, :h => 100)
  end

  it "should wrap an image" do
    image = Ray::Image.new(:w => 10, :h => 10)

    sprite = Ray::Sprite.new(image)
    sprite.image.should == image
  end

  it "should convert strings to images" do
    image = Ray::ImageSet[path_of("aqua.bmp")]

    sprite = Ray::Sprite.new(path_of("aqua.bmp"))
    sprite.image.should == image
  end

  it "should blit its image when drawn" do
    image = mock("image")
    image.stub!(:to_image).and_return(image)
    image.stub!(:is_a?).and_return(true)
    image.should_receive(:blit).and_return(@win)

    sprite = Ray::Sprite.new(image)
    sprite.draw_on(@win)
  end

  it "should draw on the screen by default" do
    image = mock("image")
    image.stub!(:to_image).and_return(image)
    image.stub!(:is_a?).and_return(true)
    image.should_receive(:blit).with(hash_including(:on => @win)).and_return(@win)

    sprite = Ray::Sprite.new(image)
    sprite.draw
  end

  it "should have a position" do
    sprite = Ray::Sprite.new(@win, :at => [10, 15])
    sprite.is_at?(10, 15).should be_true
  end

  it "should not be at a position it wan't assigned to" do
    sprite = Ray::Sprite.new(@win, :at => [13, 20])
    sprite.is_at?(10, 15).should be_false
  end

  it "should be at (0, 0) by default" do
    sprite = Ray::Sprite.new(@win)
    sprite.is_at?(0, 0).should be_true
  end

  it "should draw the image at its position" do
    image = mock("image")
    image.stub!(:to_image).and_return(image)
    image.stub!(:is_a?).and_return(true)
    image.should_receive(:blit).with(hash_including(:at => [10, 15])).
      and_return(@win)

    sprite = Ray::Sprite.new(image, :at => [10, 15])
    sprite.draw
  end

  it "should allow to draw only a small rect of the image" do
    image = mock("image")
    image.stub!(:to_image).and_return(image)
    image.stub!(:is_a?).and_return(true)
    rect = Ray::Rect.new(50, 30, 100, 120)
    image.should_receive(:blit).with(hash_including(:rect => rect)).
      and_return(@win)

    sprite = Ray::Sprite.new(image, :rect => [50, 30, 100, 120])
    sprite.draw
  end

  it "should default to an empty rect" do
    sprite = Ray::Sprite.new(@win)
    sprite.from_rect.should == Ray::Rect.new(0, 0, 0, 0)
  end

  describe "#rect" do
    it "should return the rect where the image will be drawn" do
      sprite = Ray::Sprite.new(@win, :rect => [70, 80, 80, 80],
                               :at => [50, 30])
      sprite.rect.should == Ray::Rect.new(50, 30, 80, 80)
    end

    it "should return the image size if the rect is not set" do
      sprite = Ray::Sprite.new(@win)
      sprite.rect.should == Ray::Rect.new(0, 0, 100, 100)
    end
  end

  it "should allow to set an angle" do
    sprite = Ray::Sprite.new(@win, :angle => 30)
    sprite.angle.should == 30
  end

  it "should draw using its angle" do
    image = mock("image")
    image.stub!(:to_image).and_return(image)
    image.stub!(:is_a?).and_return(true)
    image.should_receive(:blit).with(hash_including(:angle => 40))

    sprite = Ray::Sprite.new(image, :angle => 40)
    sprite.draw
  end

  it "should allow to zoom" do
    sprite = Ray::Sprite.new(@win, :zoom => 3)
    sprite.zoom.should == 3
  end

  it "should draw using its zoom level" do
    image = mock("image")
    image.stub!(:to_image).and_return(image)
    image.stub!(:is_a?).and_return(true)
    image.should_receive(:blit).with(hash_including(:zoom => 3))

    sprite = Ray::Sprite.new(image, :zoom => 3)
    sprite.draw
  end
  
  it "should allow to set its position using an array" do
    sprite = Ray::Sprite.new(@win)
    sprite.pos = [10, 15]
    sprite.pos.to_rect.should == Ray::Rect.new(10, 15)
  end
  
  it "should allow to use a sprite sheet" do
    sprite = Ray::Sprite.new(@win)
    sprite.sheet_size = [10, 10]
    
    sprite.rect.w.should == (@win.w / 10)
    sprite.rect.h.should == (@win.h / 10)
  end
  
  it "should allow to change the cell which will be drawn" do
    sprite = Ray::Sprite.new(@win)
    sprite.sheet_size = [10, 10]
    sprite.sheet_pos = [3, 6]
    
    sprite.from_rect.should == Ray::Rect.new((@win.w / 10) * 3,
                                             (@win.h / 10) * 6,
                                             @win.w / 10, @win.h / 10)
  end
  
  it "should allow to disable the use of a sprite sheet" do
    sprite = Ray::Sprite.new(@win)
    sprite.sheet_size = [10, 10]
    sprite.disable_sprite_sheet
    sprite.from_rect.should == Ray::Rect.new(0, 0, @win.w, @win.h)
  end

  after :all do
    Ray.stop
  end
end
