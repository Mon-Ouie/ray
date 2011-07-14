# Ported from the Gosu tutorial by Spooner

$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require 'ray'
require 'forwardable'

def path_of(resource)
  File.expand_path File.join(File.dirname(File.dirname(__FILE__)), resource)
end

class Player
  include Ray::Helper

  extend Forwardable
  def_delegators :@sprite, :x, :y, :x=, :y=, :pos, :pos=

  attr_reader :score, :window, :sprite

  def initialize(stars)
    @stars = stars
    @sprite = Ray::Sprite.new path_of("_media/Starfighter.png")
    @sprite.origin = @sprite.image.size / 2

    @beep = sound path_of("_media/Beep.wav")

    @vel_x = @vel_y = 0.0

    @score = 0
  end

  def register(scene)
    self.event_runner = scene.event_runner
    @window = scene.window
  end

  def update
    if holding? key(:up)
      @vel_x += Math::sin(@sprite.angle / (180 / Math::PI)) * 0.5
      @vel_y -= Math::cos(@sprite.angle / (180 / Math::PI)) * 0.5
    end

    if holding? key(:left)
      @sprite.angle -= 4.5
    end

    if holding? key(:right)
      @sprite.angle += 4.5
    end

    move

    collect_stars
  end

  def warp(position)
    @sprite.pos = position
  end

  def move
    self.pos += [@vel_x, @vel_y]
    self.x %= window.size.width
    self.y %= window.size.height

    @vel_x *= 0.95
    @vel_y *= 0.95
  end

  def collect_stars
    @stars.reject! do |star|
      if pos.dist(star.pos) < 35 then
        @score += 10
        @beep.play
        true
      else
        false
      end
    end
  end
end

class Star
  include Ray::Helper

  extend Forwardable
  def_delegators :@sprite, :x, :y, :x=, :y=, :pos

  attr_reader :sprite

  def initialize(position)
    @sprite = Ray::Sprite.new path_of("_media/Star.png"), :at => position
    @sprite.sheet_size = [10, 1]
    @sprite.origin = (@sprite.image.size / @sprite.sheet_size) / 2
    @animation = sprite_animation(:from => [0, 0], :to => [9, 0],
                                  :duration => 2).start(@sprite)

    color = Ray::Color.white
    color.red   = rand(255 - 40) + 40
    color.green = rand(255 - 40) + 40
    color.blue  = rand(255 - 40) + 40
    @sprite.color = color
  end

  def register(scene)
    self.event_runner = scene.event_runner
    @animation.event_runner = event_runner

    on :animation_end, @animation do
      @animation.start @sprite
    end
  end

  def update
    @animation.update
  end
end

Ray::Game.new("Starfighter --- [LEFT/RIGHT/UP to move; collect the stars]") do
  register do
    add_hook :quit, method(:exit!)
  end

  scene :game do
    @background_image = sprite path_of("_media/Space.png")
    @score_text = text("", :at => [10, 10], :size => 20,
                       :color => Ray::Color.yellow)

    @stars = Array.new

    @player = Player.new @stars
    @player.register self
    @player.warp(window.size / 2)

    add_hook :key_press, key(:escape), method(:exit!)

    always do
      if rand(100) < 4 and @stars.size < 25 then
        star = Star.new [window.size.width * rand, window.size.height * rand]
        star.register self
        @stars << star
      end

      @stars.each(&:update)
      @player.update

      @score_text.string = "Score: #{@player.score}"
    end

    render do |win|
      win.draw @background_image
      @stars.each {|star| win.draw star.sprite }
      win.draw @player.sprite
      win.draw @score_text
    end
  end

  scenes << :game
end
