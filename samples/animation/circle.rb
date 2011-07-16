$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require 'ray'

Ray.game "Circular motion" do
  register { add_hook :quit, method(:exit!) }

  scene :rotation do
    @sun = Ray::Polygon.circle([0, 0], 50, Ray::Color.yellow)
    @sun.pos = window.size / 2

    @earth = Ray::Polygon.circle([0, 0], 20, Ray::Color.blue)
    @animation = circular_motion(:center => @sun.pos, :angle => 360,
                                 :duration => 5, :radius => 200).start(@earth)

    @animation.loop!

    on :key_press, key(:p) do
      if @animation.running?
        @animation.pause
      else
        @animation.resume
      end
    end

    always do
      @animation.update
    end

    render do |win|
      win.draw @earth
      win.draw @sun
    end
  end

  scenes << :rotation
end
