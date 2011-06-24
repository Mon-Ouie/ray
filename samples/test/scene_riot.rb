$:.unshift File.dirname(__FILE__)
require 'actual_scene'

require 'riot'
require 'riot/rr'

context AwesomeScene do
  helper(:raise_event) { |*args| @game.raise_event(*args) }
  helper(:run_event_loop) { @game.event_runner.run }

  setup do
    @game = AwesomeGame.new
    @scene = @game.registered_scene(:awesome_scene)

    @scene.register
    @scene.setup

    @scene
  end

  asserts("cursor position") { topic.cursor.pos }.equals Ray::Vector2[0, 0]

  context "after moving the mouse" do
    hookup do
      raise_event :mouse_motion, Ray::Vector2[100, 100]
      run_event_loop
    end

    asserts("cursor position") { topic.cursor.pos }.equals Ray::Vector2[100, 100]
  end

  asserts "draws its cursor" do
    stub(topic.window).draw
    topic.render topic.window

    received(topic.window).draw(topic.cursor).call
    true
  end
end
