# Drawing code stolen from here:
#  http://www.rubyquiz.com/quiz104.html

$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require 'ray'

class Ray::Turtle
  def tree(size)
    if size < 5
      forward size
      backward size
    else
      forward size / 3

      left 30
      tree size * 2 / 3
      right 30

      forward size / 6

      right 25
      tree size / 2
      left 25

      forward size / 3
      right 25
      tree size / 2
      left 25

      forward size / 6
      backward size
    end
  end
end

Ray::Game.new("Turtle") do
  register do
    add_hook :quit, method(:exit!)
  end

  scene :tree do
    tree = Ray::Image.new window.size
    tree.turtle do
      pen_up
      left 90
      center
      backward 180
      pen_down
      tree(240.0)
    end

    @sprite = sprite tree

    render do |win|
      win.draw @sprite
    end
  end

  push_scene :tree
end
