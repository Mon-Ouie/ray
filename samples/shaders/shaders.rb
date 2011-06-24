$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require 'ray'

Ray::Game.new("Shaders test") do
  register do
    add_hook :quit, method(:exit!)
  end

  scene :shader do
    img = Ray::Image.new [100, 100]

    image_target img do |target|
      target.clear Ray::Color.green

      target.draw Ray::Polygon.line([0, 0], [100, 100], 2, Ray::Color.black)
      target.draw Ray::Polygon.line([0, 100], [100, 0], 2, Ray::Color.white)

      target.draw Ray::Polygon.circle([50, 50], 10, Ray::Color.cyan)

      target.update
    end

    shader = Ray::Shader.new :frag => StringIO.new(<<-SHADER)
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

    shader[:ratio] = [0.299, 0.587, 0.114]

    @normal_sprite = sprite img, :at => [100, 100]
    @shaded_sprite = sprite img, :shader => shader

    render do |win|
      win.draw @shaded_sprite
      win.draw @normal_sprite
    end
  end

  scenes << :shader
end
