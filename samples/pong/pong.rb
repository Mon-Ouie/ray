$: << File.expand_path(File.dirname(__FILE__) + "/../../lib")
$: << File.expand_path(File.dirname(__FILE__) + "/../../ext")

def path_of(res)
  File.expand_path(File.dirname(__FILE__) + "/../../spec/res/#{res}")
end

require 'ray'

module Pong
  class Scene < Ray::Scene
    scene_name :pong_scene

    def setup
      @font = font(path_of("VeraMono.ttf"), 12)

      player_img = Ray::Image.new(:w => 20, :h => 50)
      player_img.draw_filled_rect(Ray::Rect.new(0, 0, 20, 50), Ray::Color.blue)

      adv_img = Ray::Image.new(:w => 20, :h => 50)
      adv_img.draw_filled_rect(Ray::Rect.new(0, 0, 20, 50), Ray::Color.red)

      ball_img = Ray::Image.new(:w => 10, :h => 10)
      ball_img.draw_filled_circle([5, 5], 5, Ray::Color.white)

      @player = sprite(player_img)
      @adv    = sprite(adv_img)
      @ball   = sprite(ball_img)

      screen = Ray.screen
      width, height = screen.w, screen.h

      @screen_rect = Ray::Rect.new(0, 0, width, height)

      @player.y = (height / 2) - 25
      @adv.y    = (height / 2) - 25

      @player.x = 30
      @adv.x    = width - 30

      reset_ball

      @scores = {:player => 0, :adv => 0}
    end

    def reset_ball
      @ball.y = (@screen_rect.h / 2) - 5
      @ball.x = (@screen_rect.w / 2) - 5

      @ball_movement = [6, 6]
    end

    def register
      self.loops_per_second = 60

      on :point_gained do |by|
        @scores[by] += 1
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
          raise_event(:point_gained, :player)
        elsif @ball.x <= 0
          raise_event(:point_gained, :adv)
        end

        if @ball.collide?(@player) || @ball.collide?(@adv) ||
            @ball.y >= @screen_rect.h || @ball.y <= 0
          @ball_movement[-1] *= -1
          @ball.y += @ball_movement.last
        end

        if @ball.collide?(@player) || @ball.collide?(@adv)
          @ball_movement[0] *= -1
          @ball.x += @ball_movement.first
        end

        adv_center = @adv.y + (@adv.image.h / 2)
        if @ball.y > adv_center
          @adv.y += 4
        elsif @ball.y < adv_center
          @adv.y -= 4
        end

        @ball.x += @ball_movement.first
        @ball.y += @ball_movement.last

        need_render!
      end
    end

    def render(win)
      win.fill(Ray::Color.black)

      @player.draw
      @adv.draw
      @ball.draw

      @font.draw("#{@scores[:player]} - #{@scores[:adv]}",
                 :at => [0, 0], :on => win,
                 :color => Ray::Color.white)
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
