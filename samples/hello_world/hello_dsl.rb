$: << File.expand_path(File.dirname(__FILE__) + "/../../lib")
$: << File.expand_path(File.dirname(__FILE__) + "/../../ext")

def path_of(res)
  File.expand_path(File.dirname(__FILE__) + "/../../spec/res/#{res}")
end

require 'ray'

Ray::Game.new("Hello world!") do
  register do
    add_hook :quit, method(:exit!)
  end
  
  scene :hello do
    @font = font(path_of("VeraMono.ttf"), 12)
    
    render do |win|
      @font.draw("Hello world", :on => win, :at => [0, 0])
    end
  end
  
  push_scene :hello
end