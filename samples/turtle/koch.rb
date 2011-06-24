# Drawing code stolen from here:
#  http://www.rubyquiz.com/quiz104.html

$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require 'ray'

class Ray::Turtle
  def koch(s, n)
    if n < 1
      forward s
      return
    end

    koch(s / 3, n - 1)
    left 60
    koch(s / 3, n - 1)
    right 120
    koch(s / 3, n - 1)
    left 60
    koch(s / 3, n - 1)
  end

  def snowflake(s, n)
    3.times do
      koch(s, n)
      right 120
    end
  end
end

Ray::Game.new("Turtle") do
  register do
    add_hook :quit, method(:exit!)
  end

  scene :koch do
    img = Ray::Image.new window.size
    img.turtle do
      pen_up
      center
      backward 165
      left 90
      forward 95
      right 90

      pen_down
      snowflake(350, 4)
    end

    @sprite = sprite img

    render do |win|
      win.draw @sprite
    end
  end

  push_scene :koch
end
