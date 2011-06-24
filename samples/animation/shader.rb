$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require 'ray'

Ray::Shader.use_old!

Ray.game "Change color" do
  register do
    add_hook :quit, method(:exit!)
  end

  scene :change do
    window.shader.compile :frag => StringIO.new(<<-EOF)
      #version 110

      uniform sampler2D in_Texture;
      uniform bool in_TextureEnabled;

      uniform float progression;

      varying vec4 var_Color;
      varying vec2 var_TexCoord;

      void main() {
        if (in_TextureEnabled)
          gl_FragColor = texture2D(in_Texture, var_TexCoord) * var_Color;
        else
          gl_FragColor = var_Color;

        float stolen_green = gl_FragColor.g * progression;
        gl_FragColor.g -= stolen_green;
        gl_FragColor.r += stolen_green;
      }
    EOF

    @img = Ray::Image.new [100, 100]

    image_target(@img) do |target|
      target.clear Ray::Color.green

      target.draw Ray::Polygon.line([0, 0], [100, 100], 2, Ray::Color.blue)
      target.draw Ray::Polygon.circle([30, 70], 20, Ray::Color.white)
      target.draw Ray::Polygon.ellipse([70, 20], 30, 10, Ray::Color.new(30, 100, 50))

      target.update
    end

    @sprite = sprite @img

    @animation = block_animation :duration => 5, :block => proc { |target, progression|
      target[:progression] = progression / 100
    }

    @animation.start window.shader

    always do
      @animation.update
    end

    render { |win| win.draw @sprite }
  end

  scenes << :change
end
