$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require 'ray'

Ray.game "Get Pixels" do
  register { add_hook :quit, method(:exit!) }

  scene :main do
    on :key_press, key(:a) do
      puts "(0,   0)  == #{window[0, 0]}"
      puts "(50,  50) == #{window[50, 50]}"
      puts "(50, 430) == #{window[50, 430]}"
    end

    on :key_press, key(:p) do
      window.to_image.write "screenshot.tga"
    end

    @obj = Ray::Polygon.circle([50, 50], 10, Ray::Color.blue)

    render do
      window.clear Ray::Color.green
      window.draw @obj
    end
  end

  scenes << :main
end
