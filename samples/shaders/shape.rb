$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require 'ray'

Ray::Shader.use_old!

Ray::Game.new("Shaders test") do
  register do
    add_hook :quit, method(:exit!)
  end

  scene :shader do
    @shape = Ray::Polygon.circle([200, 200], 100, Ray::Color.black)
    @shape.each do |point|
      point.color = Ray::Color.new(rand(256), rand(256), rand(256))
    end

    @shape.shader = Ray::Shader.new :frag => StringIO.new(<<-SHADER)
      #version 110

      uniform vec3 ratio[2];
      varying vec4 var_Color;

      void main() {
        float gray   = dot(var_Color.rgb, ratio[0]);
        gl_FragColor = vec4(gray, gray, gray, 1.0);
      }
    SHADER

    @shape.shader[:ratio] = [0.299, 0.587, 0.114]

    render do |win|
      win.draw @shape
    end
  end

  push_scene :shader
end
