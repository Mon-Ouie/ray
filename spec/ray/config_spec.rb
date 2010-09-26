require File.expand_path(File.dirname(__FILE__) + '/../spec_helper')

describe Ray::Config do
  it "should allow to add new settings" do
    Ray::Config.config do |c|
      c[:val] = :obj
      c[:val].should == :obj
    end

    Ray::Config[:val].should == :obj
  end

  it "should use instance_eval if the block for .config takes no argument" do
    Ray::Config.config do
      self.class.should == Ray::Config
    end
  end

  it "should default settings to nil" do
    Ray::Config[:dont_put_a_value_here].should be_nil
  end

  it "should be reset when config! is called" do
    Ray::Config.config! do |c|
      c[:bar] = 3
    end

    Ray::Config[:bar].should == 3
    Ray::Config.config! {}
    Ray::Config[:bar].should be_nil
  end

  it "should set the value directly if the value is not a hash" do
    Ray::Config.config do |conf|
      conf.set :value, :val
      conf[:value].should == :val
    end
  end

  it "should convert the object is a hash with a known type as its only key" do
    Ray::Config.config do |conf|
      conf.set :value, :string => 3
      conf[:value].should == "3"
    end
  end

  it "should set the value directly if the key is not a known type" do
    Ray::Config.config do |conf|
      conf.set :value, 3 => "bar"
      conf[:value].should == {3 => "bar"}
    end
  end

  it "should set the value directly if the hash size is not one" do
    Ray::Config.config do |conf|
      conf.set :value, {}
      conf[:value].should == {}
    end
  end

  it "shoul set the value to nil when unset is called" do
    Ray::Config.config do |conf|
      conf[:x] = 3
      conf[:x].should == 3

      conf.unset :x
      conf[:x].should be_nil
    end
  end
end
