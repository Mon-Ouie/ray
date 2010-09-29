require File.expand_path(File.dirname(__FILE__) + '/../spec_helper')

describe Ray::DSL do
  before :each do
      @runner = Ray::DSL::EventRunner.new

      @obj = Object.new
      @obj.extend Ray::DSL::Listener
      @obj.extend Ray::DSL::EventRaiser
      @obj.extend Ray::Matchers

      @obj.listener_runner = @runner
      @obj.raiser_runner = @runner
    end

  it "should send events with the same arguments and name as in #on" do
    count = 0
    @obj.on :right_event, "danger", "bar" do |*args|
      args.should == ["danger", "bar"]
      count += 1
    end

    [:right_event, :wrong_event].each do |name|
      @obj.raise_event(name)
      @obj.raise_event(name, "bar")
      @obj.raise_event(name, "danger")
      @obj.raise_event(name, "danger", "bar")
    end

    @runner.run
    count.should == 1
  end

  it "should use === to see if an event can be raised" do
    true_matcher = mock('true_matcher')
    true_matcher.should_receive(:===).and_return(true)

    false_matcher = mock('false_matcher')
    false_matcher.should_receive(:===).and_return(false)

    worked = nil

    @obj.on :event, true_matcher do
      worked.should_not == false
      worked = true
    end

    @obj.on :event, false_matcher do
      worked = false
    end

    @obj.raise_event(:event, :argument)
    @runner.run

    worked.should be_true
  end

  it "should pass the arguments of the event to the block" do
    @obj.on :foo do |*args|
      args.should == ["a", "b", "c"]
    end

    @obj.raise_event(:foo, "a", "b", "c")
    @runner.run
  end

  it "should send events with more arguments than #on if they match" do
    count = 0
    @obj.on :foo do
      count += 1
    end

    @obj.raise_event(:foo, "blabla")
    @obj.raise_event(:foo)
    @obj.raise_event(:foo, :error, 3, "read")

    @runner.run
    count.should == 3
  end
end
