$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require 'ray'

Ray::Game.new("Shaders test") do
  register do
    add_hook :quit, method(:exit!)
  end

  scene :shader do
    @poly = Ray::Polygon.rectangle([0, 0, 100, 100], Ray::Color.black)
    @poly.each do |point|
      point.color = Ray::Color.new(rand(256), rand(256), rand(256))
    end

    window.shader.compile :frag => StringIO.new(<<-SHADER)
      #version 110

      uniform sampler2D in_Texture;
      uniform bool in_TextureEnabled;

      uniform vec3 ratio;

      varying vec4 var_Color;
      varying vec2 var_TexCoord;

      void main() {
        if (in_TextureEnabled)
         gl_FragColor = texture2D(in_Texture, var_TexCoord) * var_Color;
        else
          gl_FragColor = var_Color;

        float gray = dot(gl_FragColor.rgb, ratio);
        gl_FragColor = vec4(gray, gray, gray, 1.0);
      }
    SHADER

    window.shader[:ratio] = [0.299, 0.587, 0.114]

    render do |win|
      win.draw @poly
    end
  end

  push_scene :shader
end
