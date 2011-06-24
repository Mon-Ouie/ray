$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require 'ray'

def path_of(res)
  File.expand_path File.join(File.dirname(__FILE__), '../../test/res', res)
end

Ray.game "Resizable window", :resizable => true do
  register { add_hook :quit, method(:exit!) }

  scene :view do
    @sprite = sprite path_of("sprite.png")

    on :window_resize do
      window.view = window.default_view
    end

    render do |win|
      win.draw @sprite
    end
  end

  scenes << :view
end
