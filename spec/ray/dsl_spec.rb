describe "the event DSL" do
  context "when someone listens to an event" do
    before :each do
      @runner = Ray::DSL::EventRunner.new

      @obj = Object.new
      @obj.extend Ray::DSL::Listener
      @obj.extend Ray::DSL::EventRaiser

      @obj.listener_runner = @runner
      @obj.raiser_runner = @runner
    end

    context "without any arguments" do
      it "should always send it" do
        var = 0
        @obj.on :foo do
          var += 1
        end

        @obj.raise_event(:foo, "blabla")
        @obj.raise_event(:foo)
        @obj.raise_event(:foo, :error, 3, "read")

        @runner.run
        var.should == 3
      end
    end

    context "with some arguments" do
      it "should receive the ones with the same arguments" do
        var = 0
        @obj.on :foo, "danger", "bar" do
          var += 1
        end

        @obj.raise_event(:foo)
        @obj.raise_event(:foo, "bar")
        @obj.raise_event(:foo, "danger")
        @obj.raise_event(:foo, "danger", "bar")
        @obj.raise_event(:foo, "danger", "bar", "test")

        @runner.run
        var.should == 1
      end
    end

    context "when the proc takes arguments" do
      it "should pass the arguments given to the event" do
        @obj.on :foo do |*args|
          args.should == ["a", "b", "c"]
        end

        @obj.raise_event(:foo, "a", "b", "c")
        @runner.run
      end
    end
  end
end
