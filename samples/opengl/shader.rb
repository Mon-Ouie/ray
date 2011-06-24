$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require File.expand_path(File.dirname(__FILE__)) + "/binding.rb"
require 'ray'

Ray::Game.new("OpenGL test") do
  register do
    add_hook :quit, method(:exit!)
  end

  scene :triangle do
    shader = Ray::Shader.new StringIO.new(<<-SHADER)
      uniform vec3 ratio;

      void main() {
        vec4 color = gl_Color;
        float gray = dot(color.xyz, ratio);

        gl_FragColor = vec4(gray, gray, gray, 1.0);
      }
    SHADER

    shader["ratio"] = [0.299, 0.587, 0.114]

    render do |win|
      win.activate

      shader.bind do
        GL.begin GL::Triangles do
          GL.color(1.0, 0.0, 0.0); GL.vertex(0.0, 0.0)
          GL.color(1.0, 1.0, 0.0); GL.vertex(0.3, 0.0)
          GL.color(1.0, 1.0, 1.0); GL.vertex(0.3, 0.5)
        end
      end
    end
  end

  push_scene :triangle
end
