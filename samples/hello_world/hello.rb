$: << File.expand_path(File.dirname(__FILE__) + "/../../lib")
$: << File.expand_path(File.dirname(__FILE__) + "/../../ext")

def path_of(res)
  File.expand_path(File.dirname(__FILE__) + "/../../spec/res/#{res}")
end

require 'ray'

class HelloScene < Ray::Scene
  scene_name :hello
  
  def setup
    @font = font(path_of("VeraMono.ttf"), 12)
  end
  
  def render(win)
    @font.draw("Hello world!", :on => win, :at => [0, 0])
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