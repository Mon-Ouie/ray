Ray.init

width  = 255
height = 1

win = Ray.create_window(:width => width, :height => 255 * height)
win.fill Ray::Color.new(0, 0, 255)
win.flip

(0...256).each do |i|
  img = Ray::Image.new(:w => width, :h => height)
  img.fill Ray::Color.new(i, i, i)

  img.blit(:from => [0, 0], :to => win, :at => [0, i * height])
end

win.flip

sleep 3

Ray.stop
