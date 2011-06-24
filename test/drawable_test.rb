require File.expand_path(File.dirname(__FILE__)) + '/helpers.rb'

context "a drawable" do
  setup { Ray::Polygon.line [0, 0], [1, 1], 1, Ray::Color.blue }

  asserts(:origin).equals Ray::Vector2[0, 0]
  asserts(:scale).equals Ray::Vector2[1, 1]
  asserts(:pos).equals Ray::Vector2[0, 0]
  asserts(:z).equals 0
  asserts(:angle).equals 0
  asserts(:shader).nil

  context "after changing origin" do
    hookup { topic.origin = Ray::Vector2[10, 20] }

    asserts(:origin).equals Ray::Vector2[10, 20]
    asserts(:scale).equals Ray::Vector2[1, 1]
    asserts(:pos).equals Ray::Vector2[0, 0]
    asserts(:z).equals 0
    asserts(:angle).equals 0
  end

  context "after changing the scale" do
    hookup { topic.scale = Ray::Vector2[3, 0.5] }

    asserts(:origin).equals Ray::Vector2[0, 0]
    asserts(:scale).equals Ray::Vector2[3, 0.5]
    asserts(:pos).equals Ray::Vector2[0, 0]
    asserts(:z).equals 0
    asserts(:angle).equals 0
  end

  context "after changing pos" do
    hookup { topic.pos = Ray::Vector2[10, 20] }

    asserts(:origin).equals Ray::Vector2[0, 0]
    asserts(:scale).equals Ray::Vector2[1, 1]
    asserts(:pos).equals Ray::Vector2[10, 20]
    asserts(:z).equals 0
    asserts(:angle).equals 0
  end

  context "after changing z" do
    hookup { topic.z = -0.5 }

    asserts(:origin).equals Ray::Vector2[0, 0]
    asserts(:scale).equals Ray::Vector2[1, 1]
    asserts(:pos).equals Ray::Vector2[0, 0]
    asserts(:z).equals -0.5
    asserts(:angle).equals 0
  end

  context "after changing angle" do
    hookup { topic.angle = 60 }

    asserts(:origin).equals Ray::Vector2[0, 0]
    asserts(:scale).equals Ray::Vector2[1, 1]
    asserts(:pos).equals Ray::Vector2[0, 0]
    asserts(:z).equals 0
    asserts(:angle).equals 60
  end

  context "with several transformations" do
    hookup do
      topic.origin = [30, 40]
      topic.pos    = [10, 20]
      topic.angle  = 90
      topic.scale  = [2, 0.5]
      topic.z      = 9
    end

    # (10, 30)   => (-20, -10) (origin)
    # (-20, -10) => (-40, -5)  (scale)
    # (-40, -5)  => (5, -40)   (rotate)
    # (5, -40)   => (15, -20)  (translate)
    # (15, -20)  => (45, 20)   (cancel origin)

    asserts(:transform, [10, 30]).almost_equals(Ray::Vector3[45, 20, 9], 1e-6)

    asserts("matrix.transform (10, 30)") {
      topic.matrix.transform [10, 30]
    }.almost_equals(Ray::Vector3[45, 20, 9], 1e-6)
  end

  context "after changing shader" do
    hookup do
      topic.shader = @shader = Ray::Shader.new
    end

    asserts(:shader).equals { @shader }
  end
end

run_tests if __FILE__ == $0
