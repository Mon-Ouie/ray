$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require 'ray'

class CustomDrawable < Ray::Drawable
  include Ray::GL

  def initialize
    super
    self.vertex_count = 3
  end

  def fill_vertices
     [Ray::Vertex.new([0,  0],  Ray::Color.red),
      Ray::Vertex.new([50, 0],  Ray::Color.green),
      Ray::Vertex.new([50, 50], Ray::Color.blue)]
  end

  def render(vertex, index)
    draw_arrays :triangles, vertex, 3
  end
end

Ray.game "OpenGL test" do
  register do
    add_hook :quit, method(:exit!)
  end

  scene :triangle do
    @obj = CustomDrawable.new
    @obj.pos = window.size / 2

    render do |win|
      win.draw @obj
    end
  end

  scenes << :triangle
end
