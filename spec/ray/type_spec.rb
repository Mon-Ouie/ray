describe "Ray.resolve_type" do
  context "when given a module" do
    it "should return it" do
      Ray.resolve_type(Class).should == Class
    end
  end

  context "when given something else" do
    it "should return nil if it's not registred" do
      Ray.resolve_type(3).should == nil
    end

    it "should return a module if it is registred" do
      Ray.resolve_type(:string).should == String
    end
  end
end

describe "Ray.know_type?" do
  context "when given a module" do
    it "should return true" do
      Ray.know_type?(Class.new).should be_true
    end
  end
end
