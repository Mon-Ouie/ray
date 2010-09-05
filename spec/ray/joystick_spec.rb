require File.expand_path(File.dirname(__FILE__) + '/../spec_helper')

describe Ray::Joystick do
  describe ".handle_event" do
    context "when handle_event= is called" do
      before :all do Ray.init end

      it "should change its value" do
        Ray::Joystick.handle_event = true
        Ray::Joystick.handle_event.should be_true

        Ray::Joystick.handle_event = false
        Ray::Joystick.handle_event.should be_false
      end

       after :all do Ray.init end
    end
  end
end
