require File.expand_path(File.dirname(__FILE__) + '/../spec_helper')

describe Ray::Config do
  describe ".config" do
    it "should allow to add new settings" do
      Ray::Config.config do |c|
        c[:val] = :obj
        c[:val].should == :obj
      end

      Ray::Config[:val].should == :obj
    end

    it "should use instance_eval if the block takes no argument" do
      Ray::Config.config do
        self.class.should == Ray::Config
      end
    end
  end

  describe "'s not set settings" do
    it "should be nil" do
      Ray::Config[:dont_put_a_value_here].should be_nil
    end
  end

  describe ".config!" do
    it "should reset every setting" do
      Ray::Config.config! do |c|
        c[:bar] = 3
      end

      Ray::Config[:bar].should == 3
      Ray::Config.config! {}
      Ray::Config[:bar].should be_nil
    end
  end

  describe "#set" do
    context "if the second argument is not a hash" do
      it "should set the value directly" do
        Ray::Config.config do |conf|
          conf.set :value, :val
          conf[:value].should == :val
        end
      end
    end

    context "if the second argument is a hash" do
      context "with only one element" do
        context "if the key is a known type" do
          it "should convert the object" do
            Ray::Config.config do |conf|
              conf.set :value, :string => 3
              conf[:value].should == "3"
            end
          end
        end

        context "if the key is not a know type" do
          it "should set the value directly" do
            Ray::Config.config do |conf|
              conf.set :value, 3 => "bar"
              conf[:value].should == {3 => "bar"}
            end
          end
        end
      end

      context "with more or less than one element" do
        it "should set the value directly" do
          Ray::Config.config do |conf|
            conf.set :value, {}
            conf[:value].should == {}
          end
        end
      end
    end
  end

  describe "#unset" do
    it "should set the value to nil" do
      Ray::Config.config do |conf|
        conf[:x] = 3
        conf[:x].should == 3

        conf.unset :x
        conf[:x].should be_nil
      end
    end
  end
end
