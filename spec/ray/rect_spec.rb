describe Ray::Rect do
  [:x, :y, :width, :height].each do |meth|
    describe "##{meth}=" do
      it "should set a new value to #{meth}" do
        rect = Ray::Rect.new(0, 0)
        lambda {
          rect.send("#{meth}=", 30)
        }.should change(rect, meth).from(0).to(30)
      end

      it "should only allow numerics" do
        rect = Ray::Rect.new(0, 0)

        ["1.0", "5.d", Ray, Ray::Rect, rect].each do |obj|
          lambda {
            rect.send("#{meth}=", obj)
          }.should raise_exception(TypeError)
        end
      end
    end
  end

  describe "#initialize" do
    context "with a hash" do
      it "should not allow to pass x but not y" do
        lambda {
          Ray::Rect.new(:x => 10)
        }.should raise_exception(ArgumentError)
      end

      it "should not allow to pass width but not height" do
        lambda {
          Ray::Rect.new(:x => 10, :y => 200, :width => 300)
        }.should raise_exception(ArgumentError)
      end

      it "should require x and y to be present" do
        lambda {
          Ray::Rect.new(:width => 400, :height => 300)
        }.should raise_exception(ArgumentError)
      end

      it "should only work with numerics" do
        ["1.0", "5.d", Ray, Ray::Rect].each do |obj|
          lambda {
            Ray::Rect.new(:x => obj, :y => 30)
          }.should raise_exception(TypeError)
        end
      end
    end

    context "with separated arguments" do
      it "should not work with one integer" do
        lambda {
          Ray::Rect.new(10)
        }.should raise_exception(ArgumentError)
      end

      it "should not work with 3 integers" do
        lambda {
          Ray::Rect.new(10, 30, 40)
        }.should raise_exception(ArgumentError)
      end

      it "should only work with numerics" do
        ["1.0", "5.d", Ray, Ray::Rect].each do |obj|
          lambda {
            Ray::Rect.new(obj, 30)
          }.should raise_exception(TypeError)
        end
      end
    end
  end

  describe "#inside?" do
    context "when given the receiver as an argument" do
      it "should return true" do
        rect = Ray::Rect.new(10, 15, 20, 30)
        rect.should be_inside(rect)
      end
    end

    context "when given a rect inside the receiver" do
      it "should return true" do
        rect = Ray::Rect.new(10, 15, 30, 40)
        Ray::Rect.new(13, 17, 10, 20).should be_inside(rect)
      end
    end

    context "when given a rect colliding with the receiver" do
      it "should return false" do
        rect = Ray::Rect.new(10, 15, 30, 40)

        Ray::Rect.new(5, 10, 20, 20).should_not be_inside(rect)
        Ray::Rect.new(11, 16, 40, 50).should_not be_inside(rect)
      end
    end

    context "when given a rect outside the receiver" do
      it "should return false" do
                rect = Ray::Rect.new(10, 15, 20, 30)

        Ray::Rect.new(1, 2, 3, 5).should_not be_inside(rect)
        Ray::Rect.new(100, 150, 350, 450).should_not be_inside(rect)
      end
    end
  end

  describe "#outside?" do
    context "when given the receiver as an argument" do
      it "should return false" do
        rect = Ray::Rect.new(10, 15, 20, 30)
        rect.should_not be_outside(rect)
      end
    end

    context "when given a rect outside the receiver" do
      it "should return true" do
        rect = Ray::Rect.new(10, 15, 20, 30)

        Ray::Rect.new(1, 2, 3, 5).should be_outside(rect)
        Ray::Rect.new(100, 150, 350, 450).should be_outside(rect)
      end
    end

    context "when given a rect inside the receiver" do
      it "should return false" do
        rect = Ray::Rect.new(10, 15, 30, 40)
        Ray::Rect.new(13, 17, 10, 20).should_not be_outside(rect)
      end
    end

    context "when given a rect colliding with the receiver" do
      it "should return false" do
        rect = Ray::Rect.new(10, 15, 30, 40)

        Ray::Rect.new(5, 10, 20, 20).should_not be_outside(rect)
        Ray::Rect.new(11, 16, 40, 50).should_not be_outside(rect)
      end
    end
  end

  describe "#colllide?" do
    context "when given the receiver as an argument" do
      it "should return true" do
        rect = Ray::Rect.new(10, 15, 20, 30)
        rect.collide?(rect).should be_true
      end
    end

    context "when given a rect outside the receiver" do
      it "should return false" do
        rect = Ray::Rect.new(10, 15, 20, 30)

        Ray::Rect.new(1, 2, 3, 5).collide?(rect).should_not be_true
        Ray::Rect.new(100, 150, 350, 450).collide?(rect).should_not be_true
      end
    end

    context "when given a rect inside the receiver" do
      it "should return true" do
        rect = Ray::Rect.new(10, 15, 30, 40)
        Ray::Rect.new(13, 17, 10, 20).collide?(rect).should be_true
      end
    end

    context "when given a rect colliding with the receiver" do
      it "should return true" do
        rect = Ray::Rect.new(10, 15, 30, 40)

        Ray::Rect.new(5, 10, 20, 20).collide?(rect).should be_true
        Ray::Rect.new(11, 16, 40, 50).collide?(rect).should be_true
      end
    end
  end
end
