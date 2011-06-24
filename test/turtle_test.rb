# -*- coding: utf-8 -*-
require File.expand_path(File.dirname(__FILE__)) + '/helpers.rb'

context "a turtle" do
  image = Ray::Image.new [100, 150]
  setup { Ray::Turtle.new image }

  asserts(:drawing?)
  asserts(:pos).equals(Ray::Vector2[0, 0])
  asserts(:angle_unit).equals(:deg)
  asserts(:angle).equals(0)
  asserts(:pen_width).equals(1)
  asserts("target's image") { topic.target.image }.equals image

  asserts("setting angle unit to something else than :deg or :rad") {
    topic.angle_unit = :foo
  }.raises_kind_of ArgumentError

  context "moved to [10, 15]" do
    hookup { topic.pos = [10, 15] }
    asserts(:pos).equals(Ray::Vector2[10, 15])
  end

  context "with pen up" do
    hookup { topic.pen_up }
    denies(:drawing?)

    context "when asked to set pen down" do
      hookup { topic.pen_down }
      asserts(:drawing?)
    end
  end

  context "using radians" do
    hookup { topic.angle_unit = :rad }

    context "with angle = π" do
      hookup { topic.angle = PI }
      asserts(:angle).almost_equals(PI)

      context "when asked to use degrees" do
        hookup { topic.angle_unit = :deg }
        asserts(:angle).almost_equals(180)
      end
    end
  end

  context "using degrees" do
    hookup { topic.angle_unit = :deg }

    context "with angle = 90" do
      hookup { topic.angle = 90 }
      asserts(:angle).almost_equals(90)

      context "when asked to use radians" do
        hookup { topic.angle_unit = :rad }
        asserts(:angle).almost_equals(PI / 2)
      end
    end
  end

  context "with angle = -30" do
    hookup { topic.angle = -30 }

    context "after having moved forward by 10" do
      hookup { topic.forward 10 }

      asserts(:x).almost_equals(+cos(deg_to_rad(-30)) * 10, 10e-6)
      asserts(:y).almost_equals(-sin(deg_to_rad(-30)) * 10, 10e-6) # reversed y axis
    end
  end

  context "which was drawing" do
    hookup { stub(topic.target).draw }

    context "with pen_width = 3.4" do
      hookup { topic.pen_width = 3.4 }

      context "after having moved forward by 10" do
        hookup { topic.forward(10) }
        asserts(:target).received(:draw, is_a(Ray::Polygon))
      end
    end

    context "after having moved forward by 10" do
      hookup { topic.forward(10) }
      asserts(:target).received(:draw, is_a(Ray::Polygon))
    end

    context "and went to (100, 100)" do
      hookup { topic.go_to [100, 100] }

      asserts(:target).received(:draw, is_a(Ray::Polygon))
      asserts(:pos).equals(Ray::Vector2[100, 100])
    end

    context "and went to the center of its target" do
      hookup { topic.center }

      asserts(:target).received(:draw, is_a(Ray::Polygon))
      asserts(:pos).equals(Ray::Vector2[50, 75])
    end

    context "and went to the center of its target when using a smaller viewport" do
      hookup do
        view = topic.target.view
        view.viewport = [0, 0, 1, 1 / 1.5]
        topic.target.view = view

        topic.center
      end

      asserts(:target).received(:draw, is_a(Ray::Polygon))
      asserts(:pos).equals(Ray::Vector2[50, 50])
    end
  end

  context "which wasn't drawing" do
    hookup do
      topic.pen_up
      stub(topic.target).draw
    end

    context "after having moved forward by 10" do
      hookup { topic.forward(10) }
      denies(:target).received(:draw, anything)
    end

    context "and went to (100, 100)" do
      hookup { topic.go_to [100, 100] }

      denies(:target).received(:draw, anything)
      asserts(:pos).equals(Ray::Vector2[100, 100])
    end

    context "and went to the center of its target" do
      hookup { topic.center }

      denies(:target).received(:draw, anything)
      asserts(:pos).equals(Ray::Vector2[50, 75])
    end
  end

  context "after having turned by 10° on the right" do
    hookup { topic.right 10 }
    asserts(:angle).almost_equals(-10)
  end

  context "after having turned by 10° on the left" do
    hookup { topic.left 10 }
    asserts(:angle).almost_equals(10)
  end

  context "with random parameters" do
    hookup do
      topic.pos        = [100, 100]
      topic.color     = Ray::Color.red
      topic.angle     = 120
      topic.pen_width = 3
      topic.pen_up

      stub(topic.target).clear
    end

    context "after #reset" do
      hookup { topic.reset }

      denies(:target).received(:clear, anything)

      asserts(:pos).equals(Ray::Vector2[0, 0])
      asserts(:angle).equals(0)
      asserts(:color).equals(Ray::Color.white)
      asserts(:drawing?)
      asserts(:pen_width).equals(1)
    end

    context "after #clear" do
      hookup { topic.clear }

      asserts(:target).received(:clear, Ray::Color.none)

      asserts(:pos).equals(Ray::Vector2[0, 0])
      asserts(:angle).equals(0)
      asserts(:color).equals(Ray::Color.white)
      asserts(:drawing?)
      asserts(:pen_width).equals(1)
    end
  end
end

run_tests if __FILE__ == $0
