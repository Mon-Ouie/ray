$: << File.expand_path(File.dirname(__FILE__) + "/../../lib")
$: << File.expand_path(File.dirname(__FILE__) + "/../../ext")

require 'ray'

module Sokoban
  class InvalidLevel < StandardError; end

  class Level
    include Ray::Helper
    include Enumerable

    def self.open(filename)
      level = Kernel.open(filename) { |io| new(io) }

      if block_given?
        yield level
      else
        level
      end
    end

    def initialize(io_or_string)
      @solved = false
      @moves = []

      @content = io_or_string.is_a?(String) ? io_or_string : io_or_string.read
      parse_content
    end

    def [](x, y)
      return :empty if x < 0 || y < 0

      line = @objects[y]
      return :empty unless line

      line[x] || :empty
    end

    def []=(x, y, value)
      @objects[y][x] = value
    end

    attr_reader :character_pos

    def can_move?(direction)
      obj = next_obj = nil
      obj, next_obj = next_objects(direction)

      return true if obj == :empty || obj == :storage
      return false if obj == :wall

      # obj is a crate
      return next_obj == :empty || next_obj == :storage
    end

    def move(direction)
      make_move(direction)
    end

    def each(&block)
      return to_enum unless block_given?

      @objects.each do |ary|
        ary.each(&block)
      end
    end

    def each_with_pos
      return to_enum(:each_with_pos) unless block_given?

      @objects.each_with_index do |ary, y|
        ary.each_with_index do |obj, x|
          yield obj, x, y
        end
      end
    end

    def reset
      @moves.clear
      parse_content
    end

    def undo
      return if @moves.empty?

      move = @moves.last
      opposite_dir = case move[:direction]
                     when :left  then :right
                     when :right then :left
                     when :up    then :down
                     when :down  then :up
                     end

      x, y = character_pos
      old_pos, = next_positions(move[:direction])
      crate, = next_objects(move[:direction])

      replacement = (self[x, y] == :man_on_storage) ? :crate_on_storage : :crate

      make_move(opposite_dir, false, false)

      if move[:moved_crate]
        if crate == :crate
          self[*old_pos] = :empty
          self[x, y] = replacement
        else
          self[*old_pos] = :storage
          self[x, y] = replacement
        end
      end

      @moves.delete_at(-1)
      check_solved
    end

    def score
      @moves.size
    end

    def solved?
      @solved
    end

    private
    def char_to_object(char)
      case char
      when "@" then :man
      when "o", "$" then :crate
      when "#" then :wall
      when "." then :storage
      when "*" then :crate_on_storage
      when "+" then :man_on_storage
      when " " then :empty
      else
        raise InvalidLevel, "'#{char}' isn't a valid level character"
      end
    end

    def find_character_pos
      @objects.each_with_index do |ary, y|
        ary.each_with_index do |obj, x|
          return [x, y] if obj == :man || obj == :man_on_storage
        end
      end
    end

    def parse_content
      @objects = []

      @content.each_line do |line|
        @objects << []

        line.chomp.each_char do |char|
          @objects.last << char_to_object(char)
        end
      end

      @character_pos = find_character_pos
      self
    end

    def check_solved
      if !include? :crate
        @solved = true
        raise_event(:level_solved, self)
      else
        @solved = false
      end
    end

    def next_positions(direction)
      x, y = character_pos

      case direction
      when :left
        (1..2).map { |i| [x - i, y] }
      when :right
        (1..2).map { |i| [x + i, y] }
      when :up
        (1..2).map { |i| [x, y - i] }
      when :down
        (1..2).map { |i| [x, y + i] }
      end
    end

    def next_objects(direction)
      next_positions(direction).map { |(x, y)| self[x, y] }
    end

    def make_move(direction, check_for_solve = true, count_move = true)
      x, y = character_pos
      obj, next_obj = next_objects(direction)
      first_pos, sec_pos = next_positions(direction)

      on_storage = (self[x, y] == :man_on_storage)
      replacement = on_storage ? :storage : :empty

      @moves << {
        :direction => direction,
        :moved_crate => obj == :crate || obj == :crate_on_storage
      } if count_move

      if obj == :empty
        self[*first_pos] = :man
        self[x, y]       = replacement

        @character_pos = first_pos
      elsif obj == :storage
        self[*first_pos] = :man_on_storage
        self[x, y]       = replacement

        @character_pos = first_pos
      elsif obj == :crate
        if next_obj == :empty
          self[*sec_pos]   = :crate
          self[*first_pos] = :man
          self[x, y]       = replacement

          @character_pos = first_pos
        elsif next_obj == :storage
          self[*sec_pos]   = :crate_on_storage
          self[*first_pos] = :man
          self[x, y]       = replacement

          @character_pos = first_pos
        end
      elsif obj == :crate_on_storage
        @moves[-1][:moved_crate] = true

        if next_obj == :empty
          self[*sec_pos]   = :crate
          self[*first_pos] = :man_on_storage
          self[x, y]       = replacement

          @character_pos = first_pos
        elsif next_obj == :storage
          self[*sec_pos]   = :crate_on_storage
          self[*first_pos] = :man_on_storage
          self[x, y]       = replacement

          @character_pos = first_pos
        end
      end

      check_solved if check_for_solve
    end
  end

  class LevelScene < Ray::Scene
    scene_name :sokoban_level

    TILE_WIDTH = 32

    def setup(filename)
      @filename = filename
      @level = Level.open(filename)

      self.loops_per_second = 60
    end

    def register
      @level.event_runner = event_runner

      [:left, :right, :up, :down].each do |dir|
        on :key_press, key(dir) do
          next if @level.solved?

          if @level.can_move?(dir)
            @level.move(dir)
            need_render!
          end
        end
      end

      on :key_press, key(:r) do
        next if @level.solved?

        @level.reset
        need_render!
      end

      on :key_press, key(:u) do
        next if @level.solved?

        @level.undo
        need_render!
      end

      on :level_solved do
        puts "Level solved!"
        pop_scene
      end
    end

    def render(win)
      reset_window(win)

      @level.each_with_pos do |obj, x, y|
        case obj
        when :man then draw_man(win, x, y)
        when :crate then draw_crate(win, x, y)
        when :wall then draw_wall(win, x, y)
        when :storage then draw_storage(win, x, y)
        when :crate_on_storage then draw_crate_on_storage(win, x, y)
        when :man_on_storage then draw_man_on_storage(win, x, y)
        when :empty then draw_empty_tile(win, x, y)
        end
      end
    end

    def reset_window(win)
      win.fill(Ray::Color.black)
    end

    def draw_man(win, x, y)
      x, y = pos_to_coord(x, y)
      win.draw_filled_rect([x, y, TILE_WIDTH, TILE_WIDTH], Ray::Color.white)
    end

    def draw_crate(win, x, y)
      x, y = pos_to_coord(x, y)
      win.draw_filled_rect([x, y, TILE_WIDTH, TILE_WIDTH], Ray::Color.gray)
    end

    def draw_wall(win, x, y)
      x, y = pos_to_coord(x, y)
      win.draw_filled_rect([x, y, TILE_WIDTH, TILE_WIDTH], Ray::Color.new(91, 59, 17))
    end

    def draw_storage(win, x, y)
      x, y = pos_to_coord(x, y)
      win.draw_filled_rect([x, y, TILE_WIDTH, TILE_WIDTH], Ray::Color.yellow)
    end

    def draw_crate_on_storage(win, x, y)
      x, y = pos_to_coord(x, y)
      win.draw_filled_rect([x, y, TILE_WIDTH, TILE_WIDTH], Ray::Color.green)
    end

    def draw_man_on_storage(win, x, y)
      x, y = pos_to_coord(x, y)
      win.draw_filled_rect([x, y, TILE_WIDTH, TILE_WIDTH], Ray::Color.red)
    end

    def draw_empty_tile(win, x, y)
      x, y = pos_to_coord(x, y)
      win.draw_filled_rect([x, y, TILE_WIDTH, TILE_WIDTH], Ray::Color.black)
    end

    def pos_to_coord(x, y)
      [x * TILE_WIDTH, y * TILE_WIDTH]
    end
  end

  class Game < Ray::Game
    def initialize
      super("Sokoban")

      LevelScene.bind(self)
      push_scene(:sokoban_level, File.expand_path(File.join(File.dirname(__FILE__), "level_1")))
    end

    def register
      add_hook :quit, method(:exit!)
    end
  end
end

Sokoban::Game.new.run
