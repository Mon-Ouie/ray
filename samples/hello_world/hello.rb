$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require 'ray'

class HelloScene < Ray::Scene
  scene_name :hello

  def setup
    @text = text "Hello world!", :size => 30
  end

  def render(win)
    win.draw @text
  end
end

class HelloWorld < Ray::Game
  def initialize
    super("Hello world!")

    HelloScene.bind(self)
    push_scene :hello
  end

  def register
    add_hook :quit, method(:exit!)
  end
end

HelloWorld.new.run
