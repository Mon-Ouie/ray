$:.unshift File.dirname(__FILE__)
require 'actual_scene'

require 'test/unit'

class AwesomeSceneTest < Test::Unit::TestCase
  def setup
    @game = AwesomeGame.new
    @scene = @game.registered_scene(:awesome_scene)

    @scene.register
    @scene.setup
  end

  def test_cursor_position
    assert_equal @scene.cursor.pos, [0, 0]
  end

  def test_cursor_movement
    @game.raise_event :mouse_motion, Ray::Vector2[100, 100]
    @game.event_runner.run

    assert_equal @scene.cursor.pos, [100, 100]
  end
end
