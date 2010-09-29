require File.expand_path(File.dirname(__FILE__) + '/../spec_helper')

describe Ray::Matchers do
  it "should add matchers as public methods" do
    lambda {
      Ray.describe_matcher(:awesome_matcher) do |reg|
        lambda { |str| str =~ reg }
      end
    }.should change {
      Ray::Matchers.instance_methods.any? do |i|
        i == :awesome_matcher or i == "awesome_matcher"
      end
    }.from(false).to(true)
  end

  it "should return a Ray::DSL::Matchers for defined matchers" do
    Ray.describe_matcher(:match) do |reg|
      lambda { |str| str =~ reg }
    end

    @obj = Object.new
    @obj.extend Ray::Matchers

    @obj.instance_eval { match("foo") }.should be_a(Ray::DSL::Matcher)
  end

  describe "#where" do
    it "should return a matcher matching if the block returns true" do
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
