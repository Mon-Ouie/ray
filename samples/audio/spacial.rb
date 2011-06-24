$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

def path_of(res)
  File.expand_path(File.dirname(__FILE__) + "/../../test/res/#{res}")
end

require 'ray'

SPEED = 5

Ray.game "Spacial sounds" do
  register do
    add_hook :quit, method(:exit!)
  end

  scene :spacial do
    center = window.size / 2

    @sound = music path_of("pop.wav")
    @sound.attenuation  = 0.5
    @sound.min_distance = 10
    @sound.pos          = [center.x, center.y, 0]
    @sound.looping      = true
    @sound.pitch        = 1
    @sound.relative     = false

    @sound.play

    @rect   = Ray::Polygon.rectangle([0, 0, 10, 10], Ray::Color.blue)
    @source = Ray::Polygon.rectangle([center.x, center.y, 10, 10],
    Ray::Color.green)

    on :key_press, key(:+) do
      @sound.pitch += 0.1
    end

    on :key_press, key(:-) do
      @sound.pitch -= 0.1
    end

    always do
      @rect.x += SPEED if holding? :right
      @rect.x -= SPEED if holding? :left
      @rect.y += SPEED if holding? :down
      @rect.y -= SPEED if holding? :up

      Ray::Audio.pos = [@rect.x, @rect.y, 0]
    end

    render do |win|
      win.draw @rect
      win.draw @source
    end
  end

  scenes << :spacial
end
