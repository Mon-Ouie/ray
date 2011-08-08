$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require 'ray'

Ray::GL.core_profile  = true
Ray::GL.major_version = 3
Ray::GL.minor_version = 2

def path_of(res)
  File.expand_path(res, File.join(File.dirname(__FILE__), '../../test/res'))
end

class Model < Ray::Drawable
  include Ray::GL

  InstanceSqrt  = 120
  InstanceCount = InstanceSqrt ** 2

  Vertex = Ray::GL::Vertex.make [
    [:pos,    "in_Position", :vector3],

    [:col,         "in_Color",       :color,   true],
    [:translation, "in_Translation", :vector3, true],
  ]

  def initialize(filename)
    super()

    @buffer = Ray::GL::Buffer.new :static, Vertex

    @buffer.resize_instance InstanceCount

    n = x = 0.0
    (0...InstanceSqrt).each do
      z = 0
      (0...InstanceSqrt).each do
        translation = [(x + rand * 2 - 1) * 10, 0, -10 * (z + 1)]
        @buffer.set_instance(n, Vertex::Instance.new(Ray::Color.new(rand(256),
                                                                    rand(256),
                                                                    rand(256)),
                                                     translation))
        n += 1

        z += rand * 2
      end

      x += rand * 2
    end

    @buffer.update_instance

    @buffer.resize 12 * 3

    index = -1

    positions = []

    open(filename) do |io|
      io.each_line do |line|
        next if line[0] == "#" or line !~ /\S/

        word, *args = line.split(" ")

        case word
        when "v"  then positions << Ray::Vector3[*args.map(&:to_f)]
        when "f"  then
          args.each do |arg|
            pos, normal = arg.split("//").map { |s| s.to_i - 1 }
            @buffer[index += 1] = Vertex.new(positions[pos])
          end
        end
      end
    end

    @buffer.update

    self.matrix_proc = proc do
      c = Math.cos(@n / 100.0)
      s = Math.sin(@n / 100.0)
      Ray::Matrix.looking_at([200 + 50 * c, 100 * s, -50],
                             [500, 20, -500],
                             [0, -1, 0]).scale([4, 4, 4])
    end
  end

  def move(n)
    @n = n
    matrix_changed!
  end

  def render(first, index)
    @buffer.bind
    draw_arrays_instanced :triangles, first, @buffer.size, InstanceCount
  end
end

Ray.game "*Many* cubes!" do
  register { add_hook :quit, method(:exit!) }

  scene :teapot do
    @model = Model.new path_of("cube.obj")

    vertex_shader = <<-code
#version 150

in vec3 in_Position;
in vec3 in_Translation;
in vec4 in_Color;

uniform mat4 in_ModelView;
uniform mat4 in_Projection;

out vec4 var_Color;

void main() {
  mat4 transform = in_ModelView *
    mat4(1, 0, 0, in_Translation.x,
         0, 1, 0, in_Translation.y,
         0, 0, 1, in_Translation.z,
         0, 0, 0, 1);
  mat4 mvp = transform * in_Projection;
  gl_Position = vec4(in_Position, 1) * mvp;

  var_Color  = in_Color;
}
code

    frag_shader = <<-code
#version 150

in  vec4 var_Color;
out vec4 out_FragColor;

void main() {
  out_FragColor = var_Color;
}
code

    window.shader.apply_vertex Model::Vertex
    window.shader.compile(:vertex => StringIO.new(vertex_shader),
                          :frag   => StringIO.new(frag_shader))

    window.view = Ray::View.new Ray::Matrix.perspective(120, 480.fdiv(640), 1,
                                                        1000)

    n = 0.0
    always do
      @model.move(n += 1)
    end

    render do |win|
      win.draw @model
    end
  end

  scenes << :teapot
end

