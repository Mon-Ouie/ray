require File.expand_path(File.dirname(__FILE__)) + '/helpers.rb'

context "a vertex" do
  setup { Ray::Vertex.new }

  asserts(:pos).equals Ray::Vector2[0, 0]
  asserts(:col).equals Ray::Color.white
  asserts(:tex).equals Ray::Vector2[0, 0]

  context "after changing pos" do
    hookup { topic.pos = [1, 2] }

    asserts(:pos).equals Ray::Vector2[1, 2]
    asserts(:col).equals Ray::Color.white
    asserts(:tex).equals Ray::Vector2[0, 0]
  end

  context "after changing col" do
    hookup { topic.col = Ray::Color.red }

    asserts(:pos).equals Ray::Vector2[0, 0]
    asserts(:col).equals Ray::Color.red
    asserts(:tex).equals Ray::Vector2[0, 0]
  end

  context "after changing tex" do
    hookup { topic.tex = [3, 4] }

    asserts(:pos).equals Ray::Vector2[0, 0]
    asserts(:col).equals Ray::Color.white
    asserts(:tex).equals Ray::Vector2[3, 4]
  end
end

context "a vertex created with settings" do
  setup { Ray::Vertex.new([1, 2], Ray::Color.green, [3, 4]) }

  asserts(:pos).equals Ray::Vector2[1, 2]
  asserts(:col).equals Ray::Color.green
  asserts(:tex).equals Ray::Vector2[3, 4]
end

run_tests if __FILE__ == $0
