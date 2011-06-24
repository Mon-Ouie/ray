$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require 'ray'

Ray.game "Chained animations" do
  register { add_hook :quit, method(:exit!) }

  scene :animation_chain do
    @text = text "Hello world!", :at => [100, 100], :size => 30

    @animations = animation_combination
    @animations << color_variation(:from => Ray::Color.green,
                                   :to => Ray::Color.blue,
                                   :duration => 3)
    @animations << translation(:of => [200, 200], :duration => 3)
    @animations << scale_variation(:of => [1, 1], :duration => 3)
    @animations << rotation(:of => 180, :duration => 3)

    @reverse_animations = -@animations

    on :animation_end, @animations do
      @reverse_animations.start @text
    end

    on :animation_end, @reverse_animations do
      @animations.start @text
    end

    @animations.start @text

    always do
      @animations.update
      @reverse_animations.update
    end

    render do |win|
      win.draw @text
    end
  end

  scenes << :animation_chain
end
