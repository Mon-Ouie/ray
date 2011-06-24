# FFI is required to run this sample.
# You could use any OpenGL binding instead.
# In fact, using an FFI with hard-coded constants is probably
# not really a good idea...

$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require File.expand_path(File.dirname(__FILE__)) + "/binding.rb"
require 'ray'

Ray::Game.new("OpenGL test") do
  register do
    add_hook :quit, method(:exit!)
  end

  scene :triangle do
    Ray.screen.activate
    GL.scale(2.0, 1.0, 1.0)

    render do |win|
      win.activate

      GL.begin GL::Triangles do
        GL.color(1.0, 0.0, 0.0); GL.vertex(0.0, 0.0)
        GL.color(1.0, 1.0, 0.0); GL.vertex(0.3, 0.0)
        GL.color(1.0, 1.0, 1.0); GL.vertex(0.3, 0.5)
      end
    end
  end

  push_scene :triangle
end
