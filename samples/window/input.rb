$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require 'ray'

Ray.game "input", :resizable => true do
  register { add_hook :quit, method(:exit!) }

  scene :main do
    @obj = Ray::Polygon.rectangle [0, 0, 100, 50], Ray::Color.red

    always do
      @obj.pos = mouse_pos
    end

    on :window_resize do
      window.view = window.default_view
    end

    render do
      window.clear Ray::Color.green
      window.draw @obj
    end
  end

  scenes << :main
end
