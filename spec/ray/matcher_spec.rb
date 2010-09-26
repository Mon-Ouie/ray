require File.expand_path(File.dirname(__FILE__) + '/../spec_helper')

describe Ray::Matchers do
  it "should add matchers as publci methods" do
    lambda {
      Ray.describe_matcher(:awesome_matcher, :string) do |reg|
        lambda { |str| str =~ reg }
      end
    }.should change {
      Ray::Matchers.instance_methods.any? do |i|
        i == :awesome_matcher or i == "awesome_matcher"
      end
    }.from(false).to(true)
  end

  it "should create matchers operating on anything if target isn't set" do
    Ray.describe_matcher(:foo_matcher) do |something|
      lambda { |x| x == something }
    end

    obj = Object.new
    obj.extend Ray::Matchers

    obj.instance_eval do
      matcher = foo_matcher("")
      matcher.can_match_on?(Object).should == true
    end
  end

  it "should return a Ray::DSL::Matchers for defined matchers" do
    Ray.describe_matcher(:match, :string) do |reg|
      lambda { |str| str =~ reg }
    end

    @obj = Object.new
    @obj.extend Ray::Matchers

    @obj.instance_eval { match("foo") }.should be_a(Ray::DSL::Matcher)
  end

  describe "#where" do
    it "should return true if the block returns true" do
      obj = Object.new
      obj.extend Ray::Helper

      res = 0
      obj.instance_eval do
        self.event_runner = Ray::DSL::EventRunner.new

        on :foo, where { |x| x > 10 } do |x|
          x.should > 10
          res += 1
        end

        raise_event(:foo, 10)
        raise_event(:foo, 15)

        listener_runner.run
      end

      res.should == 1
    end
  end
end

describe Ray::DSL::Matcher do
  it "should only be able to match on a given class or module" do
    matcher = Ray::DSL::Matcher.new(:string) { |foo| false }
    matcher.can_match_on?(Array).should be_false
    matcher.can_match_on?(Object).should be_false

    matcher.can_match_on?(String).should be_true
    matcher.can_match_on?(Class.new(String)).should be_true
  end

  describe "#can_match_on?" do
    it "should resolve types" do
      matcher = Ray::DSL::Matcher.new(:string) { |foo| false }
      matcher.can_match_on?(:string).should be_true
    end
  end

  describe "#match?" do
    it "should return false if we can't match on the object's class" do
      matcher = Ray::DSL::Matcher.new(:string) { |foo| true }
      matcher.match?([]).should be_false
      matcher.match?(Object.new).should be_false

      matcher.match?("").should be_true
      matcher.match?(Class.new(String).new).should be_true
    end
  end
end
