describe "the event DSL" do
  context "when someone listens to an event" do
    before :all do
      Ray.describe_matcher(:more_than, :integer) do |x|
        lambda { |i| i > x }
      end
    end

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
        var.should == 1
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
            raise_event(:foo, "a")
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

    context "if the event was described" do
      before :all do
        Ray.describe_event(:weird_thing, :string, :integer)
      end

      it "should convert arguments at creation" do
        ev = Ray::DSL::Event.new(:weird_thing, [3, "3"])
        ev.args.should == ["3", 3]
      end

      it "should not raise an error if it can't convert objects" do
        ev = Ray::DSL::Event.new(:weird_thing, ["str", String])
        ev.args.should == ["str", String]
      end

      it "should always convert if a method like #to_i is available" do
        obj = Object.new
        class << obj; def to_i; 3; end; end

        ev = Ray::DSL::Event.new(:weird_thing, [String, obj])
        ev.args.should == ["String", 3]
      end

      it "should try to convert arguments for handlers" do
        count = 0

        @obj.instance_eval do
          on :weird_thing, 3 do |x|
            x.should == "3"
            count += 1
          end

          raise_event(:weird_thing, 0)
          raise_event(:weird_thing, 3)
          raise_event(:weird_thing, "3")
          raise_event(:weird_thing, String)
        end

        @runner.run
        count.should == 2
      end

      it "should not try to convert matchers and regexps in handlers" do
        count = 0

        Ray.describe_conversion(Ray::DSL::Matcher => :integer,
                                Regexp => :string) {}

        Ray.describe_matcher(:more_than, :integer) do |x|
          lambda { |val| val > x }
        end

        @obj.instance_eval do
          on :weird_thing, /\d/, more_than(10) do |x, y|
            count += 1
          end

          raise_event(:weird_thing, "1", 15)
        end

        @runner.run
        count.should == 1
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
