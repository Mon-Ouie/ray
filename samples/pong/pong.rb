$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

def path_of(res)
  File.expand_path(File.dirname(__FILE__) + "/../../test/res/#{res}")
end

require 'ray'

# NB: this tests Ray::ImageTarget, there's no other reason to use it.

module Pong
  class Scene < Ray::Scene
    scene_name :pong_scene

    def setup
      player_img = Ray::Image.new [20, 50]
      image_target(player_img) do |target|
        target.draw Ray::Polygon.rectangle([0, 0, 20, 50], Ray::Color.blue)
        target.update
      end

      adv_img = Ray::Image.new [20, 50]
      image_target(adv_img) do |target|
        target.clear Ray::Color.red
        target.update
      end

      ball_img = Ray::Image.new [10, 10]
      image_target(ball_img) do |target|
        target.draw Ray::Polygon.circle([5, 5], 5, Ray::Color.white)
        target.update
      end

      @player = sprite player_img
      @adv    = sprite adv_img
      @ball   = sprite ball_img

      width, height = window.size.to_a

      @screen_rect = Ray::Rect[0, 0, width, height]

      @player.y = (height / 2) - 25
      @adv.y    = (height / 2) - 25

      @player.x = 30
      @adv.x    = width - 30

      reset_ball

      @scores = {:player => 0, :adv => 0}

      @score = text "0 - 0", :size => 12
    end

    def reset_ball
      @ball.y = (@screen_rect.h / 2) - 5
      @ball.x = (@screen_rect.w / 2) - 5

      @ball_movement = Ray::Vector2[6, 6]
    end

    def register
      self.loops_per_second = 60

      on :point_gained do |by|
        @scores[by] += 1
        @score.string = "#{@scores[:player]} - #{@scores[:adv]}"
        reset_ball
      end

      always do
        if holding? key(:down)
          @player.y += 4
          @player.y -= 4 unless @player.inside? @screen_rect
        elsif holding? key(:up)
          @player.y -= 4
          @player.y += 4 unless @player.inside? @screen_rect
        end

        if @ball.x >= @screen_rect.w
          raise_event :point_gained, :player
        elsif @ball.x <= 0
          raise_event :point_gained, :adv
        end

        if @ball.collide?(@player) || @ball.collide?(@adv) ||
            @ball.y >= @screen_rect.h || @ball.y <= 0
          @ball_movement.y *= -1
        end

        if @ball.collide?(@player) || @ball.collide?(@adv)
          @ball_movement.x *= -1
        end

        if @ball_movement.x > 0
          adv_center = @adv.y + (@adv.image.h / 2)
          if @ball.y > adv_center
            @adv.y += 4
          elsif @ball.y < adv_center
            @adv.y -= 4
          end
        end

        @ball.pos += @ball_movement
      end
    end

    def render(win)
      win.clear Ray::Color.black

      win.draw @player
      win.draw @adv
      win.draw @ball

      win.draw @score
    end
  end

  class Game < Ray::Game
    def initialize
      super("Pong")

      Scene.bind(self)
      push_scene :pong_scene
    end

    def register
      add_hook :quit, method(:exit!)
    end
  end
end

Pong::Game.new.run
