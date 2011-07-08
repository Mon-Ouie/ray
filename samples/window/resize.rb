$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require 'ray'

Ray.game "Resize window" do
  register { add_hook :quit, method(:exit!) }

  scene :main do
    on :key_press, key(:a) do |text|
      window.size = [100, 100]
    end

    on :key_press, key(:b) do |text|
      window.size = [640, 480]
    end

    on :window_resize do |size|
      puts "my size is now #{size}"
    end
  end

  scenes << :main
end
