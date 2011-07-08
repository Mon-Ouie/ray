$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require 'ray'

Ray.game "Fullscreen", :fullscreen => true, :size => Ray.screen_size do
  register { add_hook :key_press, key(:q), method(:exit!) }

  scene :main do
    @text = text "Press q to leave", :size => 30

    render do |win|
      win.draw @text
    end
  end

  scenes << :main
end
