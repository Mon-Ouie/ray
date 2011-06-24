# Drawing code stolen from here:
#  http://www.rubyquiz.com/quiz104.html

$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require 'ray'

Ray::Game.new("Turtle") do
  register do
    add_hook :quit, method(:exit!)
  end

  scene :mandala do
    img = Ray::Image.new window.size
    img.turtle do
      def polygon(r, n)
        center = target.clip.center

        poly = []
        theta = 360 * (PI / 180) / n

        n.times do |k|
          angle = k * theta
          poly << [center.x + r * sin(angle),
                   center.y + r * cos(angle)]
        end

        poly
      end

      def ray(from, to)
        pen_down
        go_to to

        pen_up
        go_to from
      end

      def fan(from, others)
        others.each { |to| ray from, to }
      end

      def mandala(r, n)
        poly = polygon(r, n)

        until poly.empty?
          v = poly.shift
          go_to v
          fan v, poly
        end
      end

      pen_up
      mandala(180, 24)
    end

    @sprite = sprite img

    render do |win|
      win.draw @sprite
    end
  end

  push_scene :mandala
end
