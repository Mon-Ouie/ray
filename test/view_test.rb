require File.expand_path(File.dirname(__FILE__)) + '/helpers.rb'

context "a view" do
  setup { Ray::View.new [25, 40], [30, 40] }

  asserts(:==, Ray::View.new([25, 40], [30, 40]))
  asserts(:dup).equals { topic }

  asserts(:center).equals Ray::Vector2[25, 40]
  asserts(:size).equals   Ray::Vector2[30, 40]
  asserts(:rect).equals   Ray::Rect[10, 20, 30, 40]

  asserts(:viewport).equals Ray::Rect.new(0, 0, 1, 1)

  asserts(:matrix).equals Ray::Matrix.orthogonal(10, 40, 60, 20, -1, 1)

  context "after changing the size" do
    hookup { topic.size = [70, 80] }

    asserts(:center).equals Ray::Vector2[25, 40]
    asserts(:size).equals   Ray::Vector2[70, 80]
    asserts(:matrix).equals Ray::Matrix.orthogonal(-10, 60, 80, 0, -1, 1)
  end

  context "after changing the center" do
    hookup { topic.center = [30, 45] }

    asserts(:center).equals Ray::Vector2[30, 45]
    asserts(:size).equals   Ray::Vector2[30, 40]
  end

  context "after unzooming" do
    hookup { topic.unzoom_by [2, 2] }

    asserts(:center).equals Ray::Vector2[25, 40]
    asserts(:size).equals   Ray::Vector2[60, 80]
  end

  context "after zooming" do
    hookup { topic.zoom_by [2, 2] }

    asserts(:center).equals Ray::Vector2[25, 40]
    asserts(:size).equals   Ray::Vector2[15, 20]
  end

  context "after changing the viewport" do
    hookup { topic.viewport = [0.3, 0.3, 0.5, 0.5] }
    asserts(:viewport).equals Ray::Rect.new(0.3, 0.3, 0.5, 0.5)
  end

  context "with a custom matrix" do
    hookup do
      topic.matrix = Ray::Matrix.scale([2, 3, 4])
      topic.zoom_by [2, 3]
    end

    asserts(:matrix).equals Ray::Matrix.scale([2, 3, 4])
  end
end

run_tests if __FILE__ == $0
