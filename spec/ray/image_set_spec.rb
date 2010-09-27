require File.expand_path(File.dirname(__FILE__) + '/../spec_helper')

Ray.image_set(/^test:(red|green|blue)$/) do |col|
  %w(red green blue).include?(col).should == true
  Ray::Image.new(:w => 50, :h => 50).fill(Ray::Color.send(col))
end

describe Ray::ImageSet do
  before :all do
    Ray.init
  end

  describe ".[]" do
    it "should return a new image" do
      Ray::ImageSet["test:red"].should be_a(Ray::Image)
    end

    it "should always return the same image" do
      obj = Ray::ImageSet["test:green"]
      obj.object_id.should == Ray::ImageSet["test:green"].object_id
    end
  end

  it "should allow to remove images from cache" do
    obj = Ray::ImageSet["test:red"]
    other = Ray::ImageSet["test:blue"]
    Ray::ImageSet.delete_if { |name, img| name == "test:red" }
    obj.object_id.should_not == Ray::ImageSet["test:red"].object_id
    other.object_id.should == Ray::ImageSet["test:blue"].object_id
  end

  it "should also use Ray::Image's cache" do
    img = Ray::ImageSet[path_of("aqua.bmp")]
    img.should == Ray::Image[path_of("aqua.bmp")]
  end

  it "should also handle Ray::Image's cache" do
    img = Ray::ImageSet[path_of("aqua.bmp")]

    Ray::ImageSet.delete_if { |name, img| name == path_of("aqua.bmp") }
    img.should_not == Ray::Image[path_of("aqua.bmp")]
    img.should_not == Ray::ImageSet[path_of("aqua.bmp")]
  end

  after :all do
    Ray.stop
  end
end
