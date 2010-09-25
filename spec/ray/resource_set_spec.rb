require File.expand_path(File.dirname(__FILE__) + '/../spec_helper')

module TestSet
  include Ray::ResourceSet

  def self.missing_pattern(string)
    "unknown"
  end

  add_set(/^string:(.+)$/) do |str|
    str
  end
end

module DefaultMissingPattern
  include Ray::ResourceSet
end

module MultipleArgumentsSet
  include Ray::ResourceSet

  need_argument_count 1

  add_set(/^string:(.+)$/) do |str, times|
    str * times
  end
end

module CacheTestSet
  include Ray::ResourceSet

  add_set(/(.+)/) { |str| str }
end

describe Ray::ResourceSet do
  it "should use the block we passed to create a string" do
    TestSet["string:test"].should == "test"
  end

  it "should always return the same object for the same key" do
    str = TestSet["string:my string"]
    str.object_id.should == TestSet["string:my string"].object_id
  end

  it "should call the missing_pattern method when a string isn't matched" do
    TestSet["Hello world"].should == "unknown"
  end

  it "should raise NoPatternError for unmatched string by default" do
    lambda {
      DefaultMissingPattern["unknown"]
    }.should raise_exception(Ray::NoPatternError)
  end

  it "should allow more arguments in []" do
    lambda {
      MultipleArgumentsSet["string:test", 3]
    }.should_not raise_exception
  end

  it "should raise an error when using an invalid argument count" do
    lambda {
      TestSet["string:foo", 3]
    }.should raise_exception
  end

  it "should provide user arguments at the end" do
    MultipleArgumentsSet.add_set(/my_(.)/) do |char, times|
      char.should  == "a"
      times.should == 3

      true
    end

    MultipleArgumentsSet["my_a", 3]
  end

  it "should be able to select objects from the cache" do
    first = CacheTestSet["first"]
    sec   = CacheTestSet["sec"]

    CacheTestSet.select! { |key, val| key == "first" }

    first.object_id.should == CacheTestSet["first"].object_id
    sec.object_id.should_not == CacheTestSet["sec"].object_id
  end

  it "should be able to reject objects from the cache" do
    first = CacheTestSet["first"]
    sec   = CacheTestSet["sec"]

    CacheTestSet.reject! { |key, val| key == "first" }

    first.object_id.should_not == CacheTestSet["first"].object_id
    sec.object_id.should == CacheTestSet["sec"].object_id
  end
end
