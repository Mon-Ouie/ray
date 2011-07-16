$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require 'ray'

Ray.game "Chained animation sequences" do
  register { add_hook :quit, method(:exit!) }

  scene :animation_seq do
    @text = text "Hello world!", :at => [100, 100], :size => 30

    seq = animation_sequence
    seq << color_variation(:from     => Ray::Color.green,
                           :to       => Ray::Color.blue,
                           :duration => 1)
    seq << translation(:of => [200, 200], :duration => 2)
    seq << scale_variation(:of => [1, 1], :duration => 1)
    seq << rotation(:of => 180, :duration => 2)

    rev = seq.start(@text).bounce!

    always do
      seq.update
      rev.update
    end

    render do |win|
      win.draw @text
    end
  end

  scenes << :animation_seq
end
