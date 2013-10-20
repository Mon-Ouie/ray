# Drawing code stolen from here:
#  http://www.rubyquiz.com/quiz104.html

$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require 'ray'

class Ray::Turtle
  def byzantium(r, n)
    return if n < 1
    forward r
    right 135

    4.times do
      pen_down
      forward 2 * r * sin(Math::PI / 4)

      pen_up
      byzantium(r / 2, n - 1)
      right 90
    end

    left 135
    backward r
  end
end

Ray::Game.new("Turtle") do
  register do
    add_hook :quit, method(:exit!)
  end

  scene :byzantium do
    img = Ray::Image.new window.size
    img.turtle do
      pen_up
      center
      byzantium(100, 5)
    end

    @sprite = sprite img

    render do |win|
      win.draw @sprite
    end
  end

  push_scene :byzantium
end
