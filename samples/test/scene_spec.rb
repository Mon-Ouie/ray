$:.unshift File.dirname(__FILE__)
require 'actual_scene'

describe AwesomeScene do
  before :each do
    @game = AwesomeGame.new
    @scene = @game.registered_scene(:awesome_scene)

    @scene.register
    @scene.setup
  end

  it "has a cursor at (0, 0)" do
    @scene.cursor.pos.should == [0, 0]
  end

  it "moves its cursor after the mouse moved" do
    @game.raise_event :mouse_motion, Ray::Vector2[100, 100]
    @game.event_runner.run

    @scene.cursor.pos.should == [100, 100]
  end

  it "draws its cursor" do
    @scene.window.should_receive(:draw, @scene.cursor)
    @scene.render @scene.window
  end

  after :each do
    @scene.clean_up
  end
end
