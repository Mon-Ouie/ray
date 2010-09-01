require File.dirname(__FILE__) + '/../spec_helper'
require 'rbconfig'

describe Ray do
  describe ".create_window" do
    context "when given an unappropriate configuration" do
      it "should raise runtime error" do
        Ray.init

        lambda {
          Ray.create_window(:w => 100, :h => 100, :bpp => 1)
        }.should raise_exception(RuntimeError)

        Ray.stop
      end
    end

    context "when given a correct configuration" do
      it "should create a window of the expected size" do
        Ray.init

        win = Ray.create_window(:w => 100, :h => 50)
        win.width.should == 100
        win.height.should == 50

        win = Ray.create_window(:width => 50, :height => 100)
        win.width.should == 50
        win.height.should == 100

        Ray.stop
      end
    end
  end

  describe ".can_use_mode?" do
    context "when given an unappropriate configuration" do
      it "should return false" do
        Ray.init
        Ray.can_use_mode?(:w => 100, :h => 100, :bpp => 1).should be_false
        Ray.stop
      end
    end

    context "when given a correct configuration" do
      it "should return true" do
        Ray.init
        Ray.can_use_mode?(:w => 480, :h => 272).should be_true
        Ray.stop
      end
    end
  end

  context "after being initialized" do
    before :all do Ray.init end

    subject { Ray }

    its(:window_title) { should be_nil }
    its(:text_icon) { should be_nil }

    its(:grab_input) { should be_false }

    after :all do Ray.stop end
  end

  context "after changing the title" do
    before :all do Ray.init end

    it "should change the window title and text icon and grab the input" do
      Ray.text_icon = "foo"
      Ray.text_icon.should == "foo"

      Ray.window_title = "bar"
      Ray.window_title == "bar"

      unless RbConfig::CONFIG["target_vendor"] == "psp"
        Ray.grab_input = false
        Ray.grab_input.should be_false

        Ray.grab_input = true
        Ray.grab_input.should be_true
      end
    end

    after :all do Ray.stop end
  end
end
