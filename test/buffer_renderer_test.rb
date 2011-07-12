require File.expand_path("helpers.rb", File.dirname(__FILE__))

class MagicDrawable < Ray::Drawable
  VertexType = Ray::GL::Vertex.make [[:foo, "bar", :float]]

  def initialize
    super VertexType
    self.vertex_count = 3
  end
end

context "a buffer renderer" do
  setup { Ray::BufferRenderer.new :static, Ray::Vertex }

  asserts("copies its drawable array") do
    !(topic.drawables.equal? topic.drawables)
  end

  asserts(:drawables).empty

  context "after adding drawables" do
    obj = Ray::Polygon.circle([100,100], 50)
    hookup { topic << obj }

    asserts(:drawables).equals [obj]
  end

  asserts("adding a drawable that uses another vertex type") {
    topic << MagicDrawable.new
  }.raises_kind_of Exception
end

run_tests if __FILE__ == $0
