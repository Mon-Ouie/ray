class TestClass; end
class SubTest < TestClass; end
class OtherTest; end
class YATest; end

Ray.describe_conversion(TestClass => OtherTest) { OtherTest.new }
Ray.describe_conversion(YATest    => SubTest)   { SubTest.new   }
Ray.describe_conversion(OtherTest => SubTest)   { SubTest.new   }
Ray.describe_conversion(OtherTest => TestClass) { TestClass.new }

describe "conversions" do
  context "when the requested conversion is known" do
    it "should convert the object" do
      Ray.convert(TestClass.new, OtherTest).should be_an(OtherTest)
    end

    context "even for a sub-class of the target" do
      it "should convert the object" do
        Ray.convert(YATest.new, TestClass).should be_a(TestClass)
      end
    end

    context "and there is a direct way to convert it" do
      it "should use it" do
        obj = Ray.convert(OtherTest.new, TestClass)
        obj.should be_a(TestClass)
        obj.should_not be_a(SubTest)
      end
    end

    it "should not create an instance of a superclass of the expected type" do
      Ray.convert(OtherTest.new, SubTest).class.should_not == TestClass
    end
  end

  context "when there's not available conversion" do
    it "should raise a type error" do
      lambda {
        Ray.convert(YATest.new, OtherTest)
      }.should raise_exception(TypeError)
    end
  end
end
