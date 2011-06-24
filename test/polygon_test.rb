require File.expand_path(File.dirname(__FILE__)) + '/helpers.rb'

context "a polygon" do
  setup { Ray::Polygon.new }

  asserts(:size).equals 0
  asserts(:outline_width).equals 1

  asserts :filled?
  denies :outlined?

  asserts(:pos_of, 0).nil
  asserts(:color_of, 0).nil
  asserts(:outline_of, 0).nil

  asserts(:set_pos_of, 0, [0, 0]).raises_kind_of ArgumentError
  asserts(:set_color_of, 0, Ray::Color.white).raises_kind_of ArgumentError
  asserts(:set_outline_of, 0, Ray::Color.none).raises_kind_of ArgumentError

  context "after adding a point" do
    hookup { topic.add_point [1, 2], Ray::Color.green, Ray::Color.red }
    asserts(:size).equals 1

    asserts(:pos_of, 0).equals Ray::Vector2[1, 2]
    asserts(:color_of, 0).equals Ray::Color.green
    asserts(:outline_of, 0).equals Ray::Color.red
  end

  context "after resize" do
    hookup { topic.resize 10 }
    asserts(:size).equals 10

    context "after changing a point" do
      id = 3

      hookup do
        topic.set_pos_of(id, [3, 4])
        topic.set_color_of(id, Ray::Color.yellow)
        topic.set_outline_of(id, Ray::Color.cyan)
      end

      asserts(:pos_of, id).equals Ray::Vector2[3, 4]
      asserts(:color_of, id).equals Ray::Color.yellow
      asserts(:outline_of, id).equals Ray::Color.cyan
    end

    context "after changing all the colors" do
      hookup do
        topic.color   = Ray::Color.cyan
        topic.outline = Ray::Color.yellow
      end

      0.upto(9) do |n|
        asserts(:color_of, n).equals Ray::Color.cyan
        asserts(:outline_of, n).equals Ray::Color.yellow
      end
    end
  end

  context "after changing the outline width" do
    hookup { topic.outline_width = 3 }
    asserts(:outline_width).equals 3
  end

  context "after disabling filling" do
    hookup { topic.filled = false }
    denies :filled?
  end

  context "after enabling outline" do
    hookup { topic.outlined = true }
    asserts :outlined?
  end

  context "copied after several changes" do
    setup do
      topic.filled   = false
      topic.outlined = true

      topic.resize 10
      topic.set_color_of(3, Ray::Color.green)

      topic.pos = [1, 3]
      topic.angle = 45

      topic.dup
    end

    denies :filled?
    asserts :outlined?

    asserts(:size).equals 10

    asserts(:color_of, 3).equals Ray::Color.green

    asserts(:pos).equals Ray::Vector2[1, 3]
    asserts(:angle).equals 45
  end
end

context "a polygon point" do
  setup do
    poly = Ray::Polygon.new 1 do |p|
      p.pos     = [10, 15]
      p.color   = Ray::Color.new(1, 2, 3)
      p.outline = Ray::Color.new(4, 5, 6)
    end

    poly[0]
  end

  asserts(:pos).equals Ray::Vector2[10, 15]
  asserts(:color).equals Ray::Color.new(1, 2, 3)
  asserts(:outline).equals Ray::Color.new(4, 5, 6)

  asserts(:id).equals 0

  asserts("first position, color, and outline of the shape") {
    poly = topic.polygon
    [poly.pos_of(0), poly.color_of(0), poly.outline_of(0)]
  }.equals([Ray::Vector2[10, 15],
            Ray::Color.new(1, 2, 3),
            Ray::Color.new(4, 5, 6)])
end

run_tests if __FILE__ == $0
