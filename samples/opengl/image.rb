$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require 'ray'

class CustomSprite < Ray::Drawable
  def initialize(img)
    super Ray::Vertex

    self.vertex_count = 4
    self.textured     = true

    @img = img
  end

  def fill_vertices
    [
     Ray::Vertex.new([0,      0],      Ray::Color.white, [0, 0]),
     Ray::Vertex.new([@img.w, 0],      Ray::Color.white, [1, 0]),
     Ray::Vertex.new([@img.w, @img.h], Ray::Color.white, [1, 1]),
     Ray::Vertex.new([0,      @img.h], Ray::Color.white, [0, 1]),
    ]
  end

  def render(first, index)
    @img.bind
    Ray::GL.draw_arrays :triangle_fan, first, 4
  end
end

Ray.game "OpenGL textured", :size => [500, 500]  do
  register do
    add_hook :quit, method(:exit!)
  end

  scene :image do
    image = Ray::Image.new [50, 50]
    image_target image do |target|
      target.clear Ray::Color.blue
      target.draw Ray::Polygon.circle([25, 25], 10, Ray::Color.green)
      target.update
    end

    @sprite = CustomSprite.new image

    render do |win|
      win.draw @sprite
    end
  end

  scenes << :image
end
