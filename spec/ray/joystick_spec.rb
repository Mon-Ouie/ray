require File.expand_path(File.dirname(__FILE__) + '/../spec_helper')

describe Ray::Joystick do
  describe ".handle_event" do
      before :all do
        Ray.init
      end

      it "can be changed" do
        Ray::Joystick.handle_event = true
        Ray::Joystick.handle_event.should be_true

        Ray::Joystick.handle_event = false
        Ray::Joystick.handle_event.should be_false
      end

       after :all do
        Ray.init
    end
  end
end
