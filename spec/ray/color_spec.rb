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
end
