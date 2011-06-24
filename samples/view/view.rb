$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require 'ray'

def path_of(res)
  File.expand_path File.join(File.dirname(__FILE__), '../../test/res', res)
end

Ray.game "View" do
  register { add_hook :quit, method(:exit!) }

  scene :view do
    @sprite = sprite path_of("sprite.png")

    @normal_view = window.default_view
    @normal_view.center = @sprite.rect.size / 2

    @zoomed_view = @normal_view.dup
    @zoomed_view.viewport = [0.05, 0.05, 0.2, 0.2]
    @zoomed_view.zoom_by [0.1, 0.1]

    render do |win|
      win.with_view @normal_view do
        win.draw @sprite
      end

      win.with_view @zoomed_view do
        win.draw @sprite
      end
    end
  end

  scenes << :view
end
