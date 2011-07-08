$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require 'ray'

Ray.game "No border", :no_frame => true do
  register { add_hook :key_press, key(:q), method(:exit!) }

  scene :main do
    render do |win|
      win.clear Ray::Color.red
    end
  end

  scenes << :main
end
