$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require 'ray'

Ray.game "My Own Buffer" do
  register { add_hook :quit, method(:exit!) }

  scene :triangle do
    @buffer = Ray::GL::Buffer.new :static, Ray::Vertex

    @buffer[0] = Ray::Vertex.new([100, 100], Ray::Color.new(255, 0, 0))
    @buffer[1] = Ray::Vertex.new([200, 100], Ray::Color.new(255, 255, 0))
    @buffer[2] = Ray::Vertex.new([200, 200], Ray::Color.new(255, 255, 255))

    @buffer.update

    # We need this: projection matrix isn't set if you don't use drawables
    window.shader["in_Projection"] = window.view.matrix

    render do |win|
      win.make_current
      @buffer.bind
      Ray::GL.draw_arrays :triangles, 0, 3
    end
  end

  scenes << :triangle
end
