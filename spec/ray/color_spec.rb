require File.expand_path(File.dirname(__FILE__) + '/../spec_helper')

describe Ray::Color do
  describe "#initialize" do
    it "should accept 3 integers" do
      lambda {
        Ray::Color.new(10, 20, 30)
      }.should_not raise_exception(ArgumentError)
    end

    it "should accept 4 integers" do
      lambda {
        Ray::Color.new(10, 20, 30, 40)
      }.should_not raise_exception(ArgumentError)
    end

    it "should only work with numerics" do
      [Ray, "string", Ray::Color].each do |obj|
        lambda {
          Ray::Color.new(obj, 0, 0, obj)
        }.should raise_exception(TypeError)
      end
    end
  end

  [:r, :g, :b, :a].each do |meth|
    describe "#{meth}=" do
      it "should only work with numerics" do
        color = Ray::Color.new(0, 0, 0, 0)
        [Ray, "string", Ray::Color].each do |obj|
          lambda {
            color.send("#{meth}=", obj)
          }.should raise_exception(TypeError)
        end
      end

      it "should set a new value" do
        color = Ray::Color.new(0, 0, 0, 0)

        lambda {
          color.send("#{meth}=", 30)
        }.should change(color, meth).from(0).to(30)
      end
    end
  end

  describe "#==" do
    it "should return true if both colors are identical" do
      Ray::Color.new(10, 15, 20, 30).should == Ray::Color.new(10, 15, 20, 30)
      Ray::Color.new(10, 15, 20, 30).should_not == Ray::Color.new(1, 15, 2, 3)
    end

    it "should not raise an exception if the argument isn't a color" do
      Ray::Color.new(1, 1, 1, 1).should_not == 1
    end
  end
end
