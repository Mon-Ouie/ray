$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require 'ray'

Ray.game "Change title" do
  register { add_hook :quit, method(:exit!) }

  scene :main do
    on :text_entered do |text|
      window.title = text
    end
  end

  scenes << :main
end
