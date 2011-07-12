$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require 'ray'

Ray.game "Buffer renderer" do
  register { add_hook :quit, method(:exit!) }

  scene :many_polygons do
    @buffer = Ray::BufferRenderer.new :static, Ray::Vertex

    1000.times do
       @buffer << Ray::Polygon.circle([rand(640), rand(480)],
                                      rand(50) + 1,
                                      Ray::Color.new(rand(256), rand(256),
                                                     rand(256)))
    end

    @buffer.update

    render do |win|
      win.draw @buffer
    end
  end

  scenes << :many_polygons
end
