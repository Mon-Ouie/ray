$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require 'ray'

def path_of(res)
  File.expand_path(res, File.join(File.dirname(__FILE__), '../../test/res'))
end

class Cube < Ray::Drawable
  include Ray::GL

  Vertex = Ray::GL::Vertex.make [
    [:pos, "in_Position", :vector3],
    [:col, "in_Color",    :color]
  ]

  Size = 50

  def initialize
    super Vertex

    self.index_count  = 6 * 6
    self.vertex_count = 6 * 4
    rotate(0, 0, 0)

    @counts     = IntArray.new(*Array.new(6, 4))
    @first_ary  = IntArray.new(*Array.new(6, 0))
  end

  def rotate(x, y, z)
    mat = Ray::Matrix.translation [0, 0, -200]

    mat.rotate(x, [1, 0, 0])
    mat.rotate(y, [0, 1, 0])
    mat.rotate(z, [0, 0, 1])

    self.matrix = mat
  end

  def fill_vertices
    [
     Vertex.new([-Size, -Size, -Size], Ray::Color.green),   # Bottom
     Vertex.new([+Size, -Size, -Size], Ray::Color.green),
     Vertex.new([+Size, +Size, -Size], Ray::Color.green),
     Vertex.new([-Size, +Size, -Size], Ray::Color.green),

     Vertex.new([-Size, -Size, +Size], Ray::Color.red),     # Top
     Vertex.new([+Size, -Size, +Size], Ray::Color.red),
     Vertex.new([+Size, +Size, +Size], Ray::Color.red),
     Vertex.new([-Size, +Size, +Size], Ray::Color.red),

     Vertex.new([-Size, -Size, -Size], Ray::Color.gray),    # Left
     Vertex.new([-Size, +Size, -Size], Ray::Color.gray),
     Vertex.new([-Size, +Size, +Size], Ray::Color.gray),
     Vertex.new([-Size, -Size, +Size], Ray::Color.gray),

     Vertex.new([+Size, -Size, -Size], Ray::Color.white),   # Right
     Vertex.new([+Size, +Size, -Size], Ray::Color.white),
     Vertex.new([+Size, +Size, +Size], Ray::Color.white),
     Vertex.new([+Size, -Size, +Size], Ray::Color.white),

     Vertex.new([-Size, -Size, -Size], Ray::Color.yellow),  # Back
     Vertex.new([+Size, -Size, -Size], Ray::Color.yellow),
     Vertex.new([+Size, -Size, +Size], Ray::Color.yellow),
     Vertex.new([-Size, -Size, +Size], Ray::Color.yellow),

     Vertex.new([-Size, +Size, -Size], Ray::Color.fuschia), # Front
     Vertex.new([+Size, +Size, -Size], Ray::Color.fuschia),
     Vertex.new([+Size, +Size, +Size], Ray::Color.fuschia),
     Vertex.new([-Size, +Size, +Size], Ray::Color.fuschia),
    ]
  end

  def fill_indices(from)
    ary = Ray::GL::IntArray.new

    vertex_id = from
    6.times do |i|
      ary << vertex_id + 0
      ary << vertex_id + 1
      ary << vertex_id + 2
      ary << vertex_id + 3
      ary << vertex_id + 0
      ary << vertex_id + 2

      vertex_id += 4
    end

    ary
  end

  def render(first, index)
    draw_elements :triangles, 6 * 6, index
  end
end

Ray::Shader.use_old!

Ray.game "A Cube!" do
  register { add_hook :quit, method(:exit!) }

  scene :cube do
    @cube = Cube.new

    window.shader.apply_vertex Cube::Vertex
    window.shader.compile(:vertex => path_of("vertex3d.c"),
                          :frag   => path_of("frag3d.c"))

    window.view = Ray::View.new Ray::Matrix.perspective(90, 1, 1, 500)

    n = 0
    always do
      n += 0.01
      @cube.rotate(n * 50, n * 30, n * 90)
    end

    render do |win|
      win.draw @cube
    end
  end

  scenes << :cube
end
