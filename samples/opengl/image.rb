$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require File.expand_path(File.dirname(__FILE__)) + "/binding.rb"
require 'ray'

Ray.game "OpenGL test", :size => [500, 500]  do
  register do
    add_hook :quit, method(:exit!)
  end

  scene :triangle do
    image = Ray::Image.new(:w => 50, :h => 50).fill(Ray::Color.blue).update

    render do |win|
      win.activate

      image.bind

      GL.begin GL::Quads do
        GL.tex_coord(0.0, 0.0); GL.vertex(0, 0.5)
        GL.tex_coord(0.0, 1.0); GL.vertex(0, 0)
        GL.tex_coord(1.0, 1.0); GL.vertex(0.5, 0)
        GL.tex_coord(1.0, 0.0); GL.vertex(0.5, 0.5)
      end
    end
  end

  push_scene :triangle
end
