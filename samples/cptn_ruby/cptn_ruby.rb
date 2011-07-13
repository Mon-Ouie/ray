# Basically, the tutorial game taken to a jump'n'run perspective.

# Shows how to
#  * implement jumping/gravity
#  * implement scrolling using viewports
#  * implement a simple tile-based map
#  * load levels from primitive text files

# Some exercises, starting at the real basics:
#  0) understand the existing code!
# As shown in the tutorial:
#  2) add gamepad support
#  3) add a score as in the tutorial game
#  4) similarly, add sound effects for various events
# Exploring this game's code and Ray:
#  5) make the player wider, so he doesn't fall off edges as easily
#  6) add background music
#  7) implement parallax scrolling for the star background!
# Getting tricky:
#  8) optimize Map#draw_on so only tiles on screen are drawn (needs modulo, a pen
#     and paper to figure out)
#  9) add loading of next level when all gems are collected
# ...Enemies, a more sophisticated object system, weapons, title and credits
# screens...

$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

require 'ray'
require 'forwardable'
  
def path_of(resource)
  File.expand_path File.join(File.dirname(File.dirname(__FILE__)), resource)
end

# Player class.
class CptnRuby 
  include Ray::Helper
  
  extend Forwardable
  def_delegators :@sprite, :x, :y, :x=, :y= 
  
  # Sprite-sheet positions for the various animations.
  POSITIONS = {
      :standing => [0, 1],
      :walking1 => [1, 1],
      :walking2 => [2, 1],
      :jumping => [3, 1],
  }
  
  MOVE_SPEED = 5
  JUMP_SPEED = 20
  
  attr_reader :window # Needed so we can use #holding?
  
  def initialize(scene, map, x, y)
    self.raiser_runner = scene.raiser_runner
    self.event_runner = scene.event_runner
    
    @window = scene.window   
    @map = map
  
    @vy = 0 # Vertical velocity
   
    # Load all animation frames
    @sprite = sprite path_of("_media/CptnRuby.png")
    @sprite.origin = [@sprite.image.width / 2, @sprite.image.height / 2] # Center
    @sprite.pos = [x, y]
    @sprite.sheet_size = [4, 1] 

    @moving = :stationary
    
    @walking_animation = sprite_animation(:from => POSITIONS[:walking1],
        :to => POSITIONS[:walking2], :duration => 1)
        
    @walking_animation.start(@sprite)
    @walking_animation.pause
    
    on :animation_end, @walking_animation do
      @walking_animation.start(@sprite)
    end
    
    # Jump.
    on :key_press, key(:up) do
      if @map.solid?(x + @sprite.sprite_width / 2, y + @sprite.sprite_height + 1)
        @vy = -JUMP_SPEED
      end
    end
  end

  def draw_on(window)
    window.draw @sprite
  end
  
  def move_horizontally(x)
    step = x / x.abs  
    (x.abs).times do
      if would_fit?(step, 0)
        self.x += step
      else
        break
      end
    end
  end
    
  def move_vertically(y)
    step = y / y.abs  
    (y.abs).times do
      if would_fit?(0, step)
        self.y += step
      else
        @vy = 0 # Hit roof or floor; stop dropping/jumping.
        break
      end
    end
  end
  
  # Could the object be placed at x + offs_x/y + offs_y without being stuck?
  def would_fit?(offs_x, offs_y)
    # Check at the center/top and center/bottom for map collisions
    not @map.solid?(x + @sprite.sprite_width / 2 + offs_x, y + offs_y) and
      not @map.solid?(x + @sprite.sprite_width / 2 + offs_x, y + offs_y + @sprite.sprite_height)
  end
  
  def update
    # Move left/right.
    if holding? key(:left)
      @moving = :walking
      @sprite.flip_x = false
      move_horizontally(-MOVE_SPEED)
    elsif holding? key(:right)
      @moving = :walking
      @sprite.flip_x = true
      move_horizontally(MOVE_SPEED)
    else
      @moving = :standing 
    end
    
    if @vy < 0
      @moving = :jumping
    end
    
    # Select image depending on action.
    case @moving 
      when :standing, :jumping        
        @walking_animation.pause unless @walking_animation.paused?   
        @sprite.sheet_pos = POSITIONS[@moving]
      when :walking
        @walking_animation.resume if @walking_animation.paused?   
        @walking_animation.update        
    end
    
    # Acceleration/gravity
    # By adding 1 each frame, and (ideally) adding vy to y, the player's
    # jumping curve will be the parabole we want it to be.
    @vy += 1
    # Vertical movement
    if @vy != 0
      move_vertically(@vy)
    end
    
    collect_gems
  end
  
  def collect_gems
    # Same as in the tutorial game.
    @map.gems.reject! { |c| @sprite.collide?(c) }
  end
end

class CollectibleGem 
  include Ray::Helper
  
  extend Forwardable
  def_delegators :@sprite, :x, :y, :to_rect
  
  def initialize(scene, x, y)   
    self.raiser_runner = scene.raiser_runner
    self.event_runner = scene.event_runner
  
    @sprite = sprite path_of("_media/CptnRuby Gem.png")
    @sprite.x, @sprite.y = x, y
    @sprite.origin = @sprite.image.width / 2,  @sprite.image.height / 2
    
    @animation = rotation(:from => -30, :to => 30, :duration => 0.6)
    @reverse_animation = -@animation
        
    on :animation_end, @animation do
      @reverse_animation.start @sprite
    end

    on :animation_end, @reverse_animation do
      @animation.start @sprite
    end
    
    @animation.start(@sprite)
  end
  
  def update
    @animation.update
    @reverse_animation.update
  end
  
  def draw_on(window)
    window.draw @sprite
  end
end
  
# Map class holds and draws tiles and gems.
class Map
  include Ray::Helper

  attr_reader :width, :height, :gems
  
  TILE_SIZE = 50 # Tiles are 50x50 pixels in size.
  
  # Positions of tiles in the sprite-sheet.
  module Tiles
    SHEET_SIZE = [2, 1]
    
    GRASS = [0, 0]
    EARTH = [1, 0]
  end
  
  def initialize(scene, filename)
    @tileset = sprite path_of("_media/CptnRuby Tileset.png")
    @tileset.sheet_size = Tiles::SHEET_SIZE

    @gems = []

    lines = File.readlines(filename).map { |line| line.chomp }
    @height = lines.size
    @width = lines[0].size
    @tiles = Array.new(@width) do |x|
      Array.new(@height) do |y|
        case lines[y][x, 1]
        when '"'
          Tiles::GRASS
        when '#'
          Tiles::EARTH
        when 'x'
          @gems << CollectibleGem.new(scene, x * TILE_SIZE, y * TILE_SIZE)
          nil
        else
          nil
        end
      end
    end
  end
  
  def draw_on(window)
    # Very primitive drawing function:
    # Draws all the tiles, some off-screen, some on-screen.
    @height.times do |y|
      @width.times do |x|
        tile = @tiles[x][y]
        if tile
          # Draw the tile with an offset (tile images have some overlap)
          # Scrolling is implemented here just as in the game objects.
          @tileset.sheet_pos = tile
          @tileset.x, @tileset.y = (x * TILE_SIZE) - 5, (y * TILE_SIZE) - 5
          window.draw @tileset
        end
      end
    end
    
    @gems.each { |c| c.draw_on window }
  end
  
  def update
    @gems.each(&:update)
  end
  
  # Solid at a given pixel position?
  def solid?(x, y)
    y < 0 || @tiles[x / TILE_SIZE][y / TILE_SIZE]
  end
end

Ray::Game.new("Cptn. Ruby --- [LEFT/RIGHT/UP to play]") do
  register do
    add_hook :quit, method(:exit!)
  end

  scene :game do   
    @sky = sprite path_of("_media/Space.png")
    @map = Map.new self, path_of("_media/CptnRuby Map.txt")
    @cptn = CptnRuby.new self, @map, 400, 100
    @camera = window.default_view # Copy the default view to use as a moving camera.
    
    on :key_press, key(:escape) do
      exit!
    end
    
    always do
      @cptn.update 
      @map.update      

      # Recalculate the position of the camera.      
      camera_x = [[@cptn.x, (window.size.width / 2)].max, @map.width * Map::TILE_SIZE - window.size.width / 2].min
      camera_y = [[@cptn.y, (window.size.height / 2)].max, @map.height * Map::TILE_SIZE - window.size.height / 2].min           
      @camera.center = [camera_x, camera_y]
    end
    
    render do |win|    
      # Sky doesn't move.
      win.draw @sky

      # Draw map and objects based on the player position.
      win.with_view @camera do
        @map.draw_on win
        @cptn.draw_on win
      end
    end
  end

  scenes << :game
end
