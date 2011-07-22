$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

def path_of(res)
  File.expand_path(File.dirname(__FILE__) + "/../../test/res/#{res}")
end

require 'ray'

Ray.game "grayscale" do
  register { add_hook :quit, method(:exit!) }

  scene :shader do
    @obj = sprite path_of("sprite.png")

    effect_generator do |gen|
      gen << grayscale
      gen.build window.shader
    end

    render do |win|
      win.draw @obj
    end
  end

  scenes << :shader
end
