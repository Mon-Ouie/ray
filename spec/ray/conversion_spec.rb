require File.expand_path(File.dirname(__FILE__) + '/../spec_helper')

class TestClass; end
class SubTest < TestClass; end
class OtherTest; end
class YATest; end

Ray.describe_conversion(TestClass => OtherTest) { OtherTest.new }
Ray.describe_conversion(YATest    => SubTest)   { SubTest.new   }
Ray.describe_conversion(OtherTest => SubTest)   { SubTest.new   }
Ray.describe_conversion(OtherTest => TestClass) { TestClass.new }

describe "Ray's conversions" do
  it "should convert the object if the requested conversion is known" do
    Ray.convert(TestClass.new, OtherTest).should be_an(OtherTest)
  end

  it "should convert the object if there's a conversion to a sub-class" do
    Ray.convert(YATest.new, TestClass).should be_a(TestClass)
  end

  it "should use direct conversion if available" do
    obj = Ray.convert(OtherTest.new, TestClass)
    obj.should be_a(TestClass)
    obj.should_not be_a(SubTest)
  end

  it "should not create an instance of a superclass of the expected type" do
    Ray.convert(OtherTest.new, SubTest).class.should_not == TestClass
  end

  it "should raise a type error if there are no available conversions" do
    lambda {
      Ray.convert(YATest.new, OtherTest)
    }.should raise_exception(TypeError)
  end

  it "should return the argument if it already has the right type" do
    obj = SubTest.new
    Ray.convert(obj, TestClass).should == obj
  end
end
