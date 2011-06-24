# Drawing code stolen from here:
#  http://www.rubyquiz.com/quiz104.html

$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require 'ray'

class Ray::Turtle
  def hilbert(s, n, k)
    return if n < 1
    left k * 90
    hilbert(s, n - 1, -k)
    forward s
    right k * 90
    hilbert(s, n - 1, k)
    forward s
    hilbert(s, n - 1, k)
    right k * 90
    forward s
    hilbert(s, n - 1, -k)
    left k * 90
  end
end

Ray::Game.new("Turtle") do
  register do
    add_hook :quit, method(:exit!)
  end

  scene :hilbert do
    img = Ray::Image.new window.size
    img.turtle do
      pen_up
      left 90
      backward 185
      right 90
      forward 185
      left 90

      pen_down
      hilbert(10, 5, 1)
    end

    @sprite = sprite img

    render do |win|
      win.draw @sprite
    end
  end

  push_scene :hilbert
end
