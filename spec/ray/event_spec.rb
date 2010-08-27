describe "the event DSL" do
  context "when someone listens to an event" do
    before :each do
      @runner = Ray::DSL::EventRunner.new

      @obj = Object.new
      @obj.extend Ray::DSL::Listener
      @obj.extend Ray::DSL::EventRaiser
      @obj.extend Ray::Matchers

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
      it "should send the ones with the same arguments" do
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
        var.should == 2
      end

      context "if a matcher is one of them" do
        it "should send the events if it matches the argument" do
          count = 0

          @obj.instance_eval do
            on :foo, more_than(10) do |x|
              x.should > 10
              count += 1
            end

            raise_event(:foo, 5)
            raise_event(:foo, 15)
          end

          @runner.run
          count.should == 1
        end
      end

      context "if a regex is one of them" do
        it "should send the event if a string match it" do
          count = 0

          @obj.instance_eval do
            on :foo, /hello/ do |str|
              str.should =~ /hello/
              count += 1
            end

            raise_event(:foo, 0)
            raise_event(:foo, "wat?")
            raise_event(:foo, "Hey, hello you!")
            raise_event(:foo, /hey hello you/)
          end

          @runner.run
          count.should == 1
        end

        it "should send then event if the argument is the same regex" do
          count = 0

          @obj.instance_eval do
            on :foo, /hello/ do |reg|
              reg.should == /hello/
              count += 1
            end

            raise_event(:foo, 0)
            raise_event(:foo, 5.5)
            raise_event(:foo, "Goodbye!")
            raise_event(:foo, /hello/)
          end

          @runner.run
          count.should == 1
        end
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
