$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require 'ray'

def path_of(res)
  File.expand_path(res, File.join(File.dirname(__FILE__), '../../test/res'))
end

class Model < Ray::Drawable
  include Ray::GL

  Vertex = Ray::GL::Vertex.make [
    [:pos,    "in_Position", :vector3],
    [:normal, "in_Normal",   :vector3],
    [:col,    "in_Color",    :color]
  ]

  def initialize(filename)
    super Vertex

    @vertices  = []

    positions = []
    normals   = []

    open(filename) do |io|
      io.each_line do |line|
        next if line[0] == "#" or line !~ /\S/

        word, *args = line.split(" ")

        case word # lacks many features.
        when "v"  then positions << Ray::Vector3[*args.map(&:to_f)]
        when "vn" then normals   << Ray::Vector3[*args.map(&:to_f)]
        when "f"  then
          args.each do |arg|
            pos, normal = arg.split("//").map { |s| s.to_i - 1 }
            @vertices << Vertex.new(positions[pos],
                                    normals[normal],
                                    Ray::Color.gray)
          end
        end
      end
    end

    self.vertex_count = @vertices.size
    rotate(0, 0, 0)
  end

  def rotate(x, y, z)
    mat = Ray::Matrix.translation [0, 0, -5]

    mat.rotate(x, [1, 0, 0])
    mat.rotate(y, [0, 1, 0])
    mat.rotate(z, [0, 0, 1])

    self.matrix = mat
  end

  def fill_vertices
    @vertices
  end

  def render(first)
    draw_arrays :triangles, first, @vertices.size
  end
end

Ray::Shader.use_old!

Ray.game "A teapot!" do
  register { add_hook :quit, method(:exit!) }

  scene :teapot do
    @model = Model.new path_of("teapot.obj")

    window.shader.apply_vertex Model::Vertex
    window.shader.compile(:vertex => path_of("light3d.c"),
                          :frag   => path_of("frag3d.c"))

    window.shader["in_Light"]      = Ray::Vector3[0, 1, 0]
    window.shader["in_LightColor"] = Ray::Color.white

    window.view = Ray::View.new Ray::Matrix.perspective(90, 1, 1, 10)

    n = 0
    always do
      n += 0.01
      @model.rotate(n * 50, n * 30, n * 90)
    end

    render do |win|
      win.draw @model
    end
  end

  scenes << :teapot
end

