# -*- coding: utf-8 -*-
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../ext")

require 'ray'

Ray.game "run_scene", :resizable => true do
  register { add_hook :quit, method(:exit!) }

  scene :main do
    [
     :quit,
     :focus_gain, :focus_loss,
     :mouse_entered, :mouse_left,
     :mouse_motion, :wheel_motion,
     :mouse_press, :mouse_release,
     :key_press, :key_release,
     :window_resize,
     :text_entered
    ].each do |name|
      name_array = [name]
      on name do |*args|
        puts "received event: #{(name_array + args).inspect}"
      end
    end

    on :window_resize do
      window.view = window.default_view
    end

    on :text_entered do |text|
      puts text.encode("utf-8")
    end

    @obj = Ray::Polygon.rectangle [50, 100, 100, 50], Ray::Color.red

    render do
      window.clear Ray::Color.green
      window.draw @obj
    end
  end

  scenes << :main
end
