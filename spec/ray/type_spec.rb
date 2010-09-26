require File.expand_path(File.dirname(__FILE__) + '/../spec_helper')

describe "Ray.resolve_type" do
  it "should return the argument if it's a module" do
    Ray.resolve_type(Class).should == Class
  end

  it "should return nil if it the type is not registred" do
    Ray.resolve_type(3).should == nil
  end

  it "should return a module if it is registred" do
    Ray.resolve_type(:string).should == String
  end
end

describe "Ray.know_type?" do
  it "should return true for modules" do
    Ray.know_type?(Class.new).should be_true
  end

  it "should return true for known types" do
    Ray.know_type?(:string).should be_true
  end

  it "should return false for unknown types" do
    Ray.know_type?(3).should be_false
  end
end
