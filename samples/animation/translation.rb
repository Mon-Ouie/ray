$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require 'ray'

Ray.game "Hello world!" do
  register { add_hook :quit, method(:exit!) }

  scene :rotation do
    @text = text "Hello world!", :at => [100, 100], :size => 30
    @translation = translation(:of => [150, 200], :duration => 1).start(@text)
    @scale       = scale_variation(:of => [0.5, 0.5], :duration => 0.5).start(@text)

    always do
      @translation.update
      @scale.update
    end

    render { |win| win.draw @text }
  end

  scenes << :rotation
end
