$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require 'ray'

def path_of(res)
  File.expand_path(res, File.join(File.dirname(__FILE__), '../../test/res'))
end

class CustomDrawable < Ray::Drawable
  include Ray::GL

  def initialize
    super
    self.vertex_count = 3
    @custom_shader = Ray::Shader.new(:vertex => path_of("vert.c"),
                                     :frag   => path_of("frag_gray.c"))
  end

  def fill_vertices
     [Ray::Vertex.new([0,   0],   Ray::Color.red),
      Ray::Vertex.new([0.5, 0],   Ray::Color.green),
      Ray::Vertex.new([0.5, 0.5], Ray::Color.blue)]
  end

  def render(vertex, index)
    @custom_shader.bind
    @custom_shader["in_ModelView"]  = Ray::Matrix.identity
    @custom_shader["in_Projection"] = matrix
    draw_arrays :triangles, vertex, 3
  end
end


Ray.game "OpenGL test" do
  register do
    add_hook :quit, method(:exit!)
  end

  scene :triangle do
    @obj = CustomDrawable.new

    render do |win|
      win.draw @obj
    end
  end

  scenes << :triangle
end
