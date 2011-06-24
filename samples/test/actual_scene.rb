$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require 'ray'

class AwesomeScene < Ray::Scene
  scene_name :awesome_scene

  def setup
    @cursor = Ray::Polygon.rectangle [-10, -10, 20, 20], Ray::Color.red
  end

  def register
    on :mouse_motion do |pos|
      @cursor.pos = pos
    end
  end

  def render(win)
    win.draw @cursor
  end

  attr_reader :cursor
end

class AwesomeGame < Ray::Game
  def initialize
    super "Hello world!", :size => [600, 600]

    AwesomeScene.bind(self)
    scenes << :awesome_scene
  end

  def register
    add_hook :quit, method(:exit!)
  end
end

if __FILE__ == $0
  AwesomeGame.new.run
end
