$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

def path_of(res)
  File.expand_path(File.dirname(__FILE__) + "/../../test/res/#{res}")
end

require 'ray'

class Sprites < Ray::Drawable
  Radius = 300

  def initialize(image)
    super()

    @image = image
    self.vertex_count = 400
  end

  def fill_vertices
    Array.new(vertex_count) do
      angle  = rand * 2 * Math::PI
      radius = Math.sqrt(rand) * Radius

      pos = [320 + Math.cos(angle) * radius, 240 + Math.sin(angle) * radius]
      Ray::Vertex.new(pos,
                      Ray::Color.new(rand(256), rand(256), rand(256)))
    end
  end

  def render(first, index)
    @image.bind
    Ray::GL.draw_arrays :points, first, vertex_count
  end
end

Ray.game "Geometry shader" do
  register { add_hook :quit, method(:exit!) }

  scene :shader do
    @sprites = Sprites.new image(path_of("stone.png"))
    @sprites.origin = window.size / 2
    @sprites.pos    = window.size / 2

    vertex = <<-vertex
#version 150

in vec2 in_Vertex;
in vec4 in_Color;
in vec2 in_TexCoord;

uniform mat4 in_ModelView;
uniform mat4 in_Projection;

out vec4 geom_Color;
out vec2 geom_TexCoord;

void main() {
  gl_Position   = vec4(in_Vertex, 0, 1) * (in_ModelView * in_Projection);
  geom_Color    = in_Color;
  geom_TexCoord = in_TexCoord;
}
vertex

    geometry = <<-geometry
#version 150

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec4 geom_Color[];
in vec2 geom_TexCoord[];

out vec4 var_Color;
out vec2 var_TexCoord;

uniform float square_size;

const float pixel_width  = 1.0 / 640;
const float pixel_height = 1.0 / 480;

struct vertex {
  vec4 pos;
  vec4 color;
  vec2 tex_coord;
};

void emit_vertex(vertex v) {
  gl_Position  = v.pos;
  var_Color    = v.color;
  var_TexCoord = v.tex_coord;
  EmitVertex();
}

void main() {
  float width  = pixel_width  * square_size / 2;
  float height = pixel_height * square_size / 2;

  vec4 position = gl_in[0].gl_Position;
  vec4 color    = geom_Color[0];

  vertex vertices[4];

  vertices[0].pos       = position + vec4(-width, -height, 0, 0);
  vertices[0].tex_coord = vec2(0, 0);
  vertices[0].color     = color;

  vertices[1].pos       = position + vec4(-width, +height, 0, 0);
  vertices[1].tex_coord = vec2(0, 1);
  vertices[1].color     = color;

  vertices[2].pos       = position + vec4(+width, -height, 0, 0);
  vertices[2].tex_coord = vec2(1, 0);
  vertices[2].color     = color;

  vertices[3].pos       = position + vec4(+width, +height, 0, 0);
  vertices[3].tex_coord = vec2(1, 1);
  vertices[3].color     = color;

  for (int i = 0; i < 4; i++)
    emit_vertex(vertices[i]);
  EndPrimitive();
}
geometry


    frag = <<-frag
#version 150

uniform sampler2D in_Texture;
uniform bool in_TextureEnabled;

in vec4 var_Color;
in vec2 var_TexCoord;

out vec4 out_FragColor;

void main() {
  out_FragColor = var_Color * texture2D(in_Texture, var_TexCoord);
}
frag

    window.shader.compile(:frag     => StringIO.new(frag),
                          :vertex   => StringIO.new(vertex),
                          :geometry => StringIO.new(geometry))

    window.shader[:square_size] = @size = 15

    always do
      if holding? :+
        window.shader[:square_size] = (@size += 3)
      elsif holding? :-
        window.shader[:square_size] = (@size -= 3)
      end

      @sprites.angle += 1.0
    end

    render do |win|
      win.draw @sprites
    end
  end

  scenes << :shader
end
