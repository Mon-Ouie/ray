$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require 'ray'

Ray::Game.new("Hello world!") do
  register do
    add_hook :quit, method(:exit!)
  end

  scene :hello do
    @text = text "Hello world!", :size => 30

    render do |win|
      win.draw @text
    end
  end

  scenes << :hello
end
