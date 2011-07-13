$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require 'ray'

class Triangles < Ray::Drawable
  def initialize
    super
    self.vertex_count = 3
  end

  def fill_vertices
    [
     Ray::Vertex.new([0, 0], Ray::Color.red),
     Ray::Vertex.new([100, 0], Ray::Color.green),
     Ray::Vertex.new([100, 200], Ray::Color.blue),
    ]
  end

  def render(first)
    Ray::GL.draw_arrays :triangles, first, 3
  end
end

Ray.game "Geometry shader" do
  register { add_hook :quit, method(:exit!) }

  scene :shader do
    @triangles = Triangles.new

    geometry = <<-geometry
#version 150

layout(triangles) in;
layout(triangle_strip, max_vertices = 6) out;

void main() {
  for(int i = 0; i < gl_in.length(); i++) {
    gl_Position = gl_in[i].gl_Position;
    EmitVertex();
  }
  EndPrimitive();

  for(int i = 0; i < gl_in.length(); i++) {
    gl_Position = gl_in[i].gl_Position + vec4(0.3, 0, 0, 0);
    EmitVertex();
  }
  EndPrimitive();
}
geometry

    vertex = <<-vertex
#version 150

in vec2 in_Vertex;
in vec4 in_Color;
in vec2 in_TexCoord;

uniform mat4 in_ModelView;
uniform mat4 in_Projection;

out vec4 var_Color;
out vec2 var_TexCoord;

void main() {
  gl_Position  = vec4(in_Vertex, 0, 1) * (in_ModelView * in_Projection);
  var_Color    = in_Color;
  var_TexCoord = in_TexCoord;
}
vertex

    frag = <<-frag
#version 150

uniform sampler2D in_Texture;
uniform bool in_TextureEnabled;

in vec4 var_Color;
in vec2 var_TexCoord;

out vec4 out_FragColor;

void main() {
  out_FragColor = vec4(0, 1, 1, 1);
}
frag

    window.shader.compile(:frag     => StringIO.new(frag),
                          :vertex   => StringIO.new(vertex),
                          :geometry => StringIO.new(geometry))

    render do |win|
      win.draw @triangles
    end
  end

  scenes << :shader
end
