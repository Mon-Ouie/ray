# Basically, the tutorial game taken to a jump'n'run perspective. (Actually,
# taken from Gosu)
#
# Shows how to
#  * implement jumping/gravity
#  * implement scrolling using views
#  * implement a simple tile-based map
#  * load levels from primitive text files
#
# Some exercises, starting at the real basics:
#  1) understand the existing code!
# As shown in the tutorial:
#  2) add gamepad support
#  3) add a score as in the tutorial game
#  4) similarly, add sound effects for various events
# Exploring this game's code and Ray:
#  5) make the player wider, so he doesn't fall off edges as easily
#  6) add background music
#  7) implement parallax scrolling for the star background!
# Getting tricky:
#  8) optimize Map#draw_on so only tiles on screen are drawn (needs modulo, a
#     pen and paper to figure out)
#  9) add loading of next level when all gems are collected
# ...Enemies, a more sophisticated object system, weapons, title and credits
# screens...

$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require 'ray'

def path_of(resource)
  File.join(File.dirname(__FILE__), "../../test/res", resource)
end

class CptnRuby
  include Ray::Helper

  Positions = {
    :standing => Ray::Vector2[0, 0],
    :walking1 => Ray::Vector2[1, 0],
    :walking2 => Ray::Vector2[2, 0],
    :jumping  => Ray::Vector2[3, 0],
  }

  MoveSpeed = 5
  JumpSpeed = 20

  def initialize(map, pos)
    @map = map
    @vy  = 0

    @sprite = Ray::Sprite.new path_of("CptnRuby.png"), :at => pos
    @sprite.sheet_size = [4, 1]

    @size = Ray::Vector2[@sprite.sprite_width, @sprite.sprite_height]

    @walking_animation = sprite_animation(:from     => Positions[:walking1],
                                          :to       => Positions[:walking2],
                                          :duration => 1).start(@sprite)
    @walking_animation.pause
  end

  def register(scene)
    self.event_runner = @walking_animation.event_runner = scene.event_runner
    @window           = scene.window

    on :animation_end, @walking_animation do
      @walking_animation.start @sprite
    end

    on :key_press, key(:up) do
      if @map.solid?(@sprite.x + @sprite.sprite_width / 2,
                     @sprite.y + @sprite.sprite_height + 1)
        @vy = -JumpSpeed
      end
    end
  end

  def update
    if holding? :left
      @state         = :walking
      @sprite.flip_x = false
      move_horizontally(-MoveSpeed)
    elsif holding? :right
      @state         = :walking
      @sprite.flip_x = true
      move_horizontally(+MoveSpeed)
    else
      @state = :standing
    end

    @state = :jumping if @vy < 0

    case @state
    when :standing, :jumping
      @walking_animation.pause unless @walking_animation.paused?
      @sprite.sheet_pos = Positions[@state]
    when :walking
      @walking_animation.resume if @walking_animation.paused?
      @walking_animation.update
    end

    # Acceleration/gravity
    # By adding 1 each frame, and (ideally) adding vy to y, the player's
    # jumping curve will be the parabole we want it to be.
    move_vertically @vy unless (@vy += 1).zero?

    @map.remove_gems { |gem| @sprite.collide? gem.sprite }
  end

  def pos; @sprite.pos; end
  def x; pos.x; end
  def y; pos.y; end

  attr_reader :sprite
  attr_reader :window

  private
  def move_horizontally(x)
    step = x / x.abs
    x.abs.times do
      if would_fit?(step, 0)
        @sprite.x += step
      else
        break
      end
    end
  end

  def move_vertically(y)
    step = y / y.abs
    y.abs.times do
      if would_fit?(0, step)
        @sprite.y += step
      else
        @vy = 0 # Hit roof or floor; stop dropping/jumping.
        break
      end
    end
  end

  def would_fit?(x, y)
    !(@map.solid?(@sprite.x + @size.w / 2 + x, @sprite.y + y) ||
      @map.solid?(@sprite.x + @size.w / 2 + x, @sprite.y + y + @size.h))
  end
end

class CollectibleGem
  include Ray::Helper

  def initialize(pos)
    @sprite = Ray::Sprite.new path_of("CptnRuby Gem.png"), :at => pos
    @sprite.origin = @sprite.image.size / 2

    @animation = rotation(:from => -30, :to => 30, :duration => 0.6)
    @reverse_animation = -@animation
  end

  def register(scene)
    self.event_runner = scene.event_runner
    @animation.event_runner = @reverse_animation.event_runner = event_runner

    on :animation_end, @animation do
      @reverse_animation.start @sprite
    end

    on :animation_end, @reverse_animation do
      @animation.start @sprite
    end

    @animation.start @sprite
  end

  def update
    @animation.update
    @reverse_animation.update
  end

  attr_reader :sprite
end

class Map
  Tileset  = path_of("CptnRuby Tileset.png")
  PartSize = 60
  TileSize = 50

  def initialize(file)
    @tiles = {}
    @gems  = []

    File.foreach(file).with_index do |line, y|
      @max_y = y

      line.each_char.with_index do |char, x|
        @max_x = x

        case char
        when ?"
          @tiles[[x, y]] = Ray::Sprite.new(Tileset, :at => [x * TileSize - 5,
                                                            y * TileSize - 5])
          @tiles[[x, y]].sub_rect = [0, 0, PartSize, PartSize]
        when ?#
          @tiles[[x, y]] = Ray::Sprite.new(Tileset, :at => [x * TileSize - 5,
                                                            y * TileSize - 5])
          @tiles[[x, y]].sub_rect = [PartSize, 0, PartSize, PartSize]
        when ?x
          @gems << CollectibleGem.new([x * TileSize + TileSize / 2,
                                       y * TileSize + TileSize / 2])
        end
      end
    end

    @max_x *= TileSize
    @max_y *= TileSize
  end

  def each_tile
    @tiles.each { |_, tile| yield tile }
  end

  def each_gem(&block)
    @gems.each(&block)
  end

  def remove_gems(&block)
    @gems.delete_if(&block)
  end

  def solid?(x, y)
    y < 0 || @tiles[[x.to_i / TileSize, y.to_i / TileSize]]
  end

  attr_reader :max_x, :max_y
end

Ray.game "Captain Ruby" do
  register { add_hook :quit, method(:exit!) }

  scene :game do
    @half_size = window.size / 2

    @sky  = sprite path_of("Space.png")

    @map  = Map.new path_of("CptnRuby Map.txt")
    @cptn = CptnRuby.new(@map, [400, 100])

    @camera = Ray::View.new @cptn.pos, window.size

    add_hook :key_press, key(:escape), method(:exit!)
    @map.each_gem { |gem| gem.register self }
    @cptn.register self

    always do
      @map.each_gem(&:update)
      @cptn.update

      # Center camera
      camera_x = [[@cptn.x, @half_size.w].max, @map.max_x - @half_size.w].min
      camera_y = [[@cptn.y, @half_size.h].max, @map.max_y - @half_size.h].min

      @camera.center = [camera_x, camera_y]
    end

    render do |win|
      win.draw @sky

      win.with_view @camera do # Apply scrolling
        @map.each_tile { |tile| win.draw tile       }
        @map.each_gem  { |gem|  win.draw gem.sprite }

        win.draw @cptn.sprite
      end
    end
  end

  scenes << :game
end
