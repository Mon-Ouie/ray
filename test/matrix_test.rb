require File.expand_path(File.dirname(__FILE__)) + '/helpers.rb'

context "a matrix" do
  setup { Ray::Matrix.new }

  asserts(:content).equals {
    [1, 0, 0, 0,
     0, 1, 0, 0,
     0, 0, 1, 0,
     0, 0, 0, 1]
  }

  asserts_topic.equals Ray::Matrix.new

  asserts(:[], 4, 0).raises_kind_of RangeError
  asserts(:[], 0, 5).raises_kind_of RangeError

  asserts(:[]=, 4, 0, 1).raises_kind_of RangeError
  asserts(:[]=, 0, 5, 1).raises_kind_of RangeError

  context "after changing a single element" do
    hookup { topic[2, 2] = 10 }

    asserts(:[], 2, 2).equals 10
    denies(:[],  2, 3).equals 10

    asserts(:content).equals {
      [1, 0, 0,  0,
       0, 1, 0,  0,
       0, 0, 10, 0,
       0, 0, 0,  1]
    }
  end

  context "after changing its whole content" do
    new_content = (1..16).to_a

    hookup { topic.content = new_content }

    asserts(:content).equals new_content
    asserts_topic.equals Ray::Matrix.new(new_content)

    asserts(:hash).equals Ray::Matrix.new(new_content).hash
    asserts(:eql?, Ray::Matrix.new(new_content))

    asserts(:dup).equals { topic }

    context "after reset" do
      hookup { topic.reset }

      asserts(:content).equals {
        [1, 0, 0, 0,
         0, 1, 0, 0,
         0, 0, 1, 0,
         0, 0, 0, 1]
      }
    end

    context "multiplied by another matrix" do
      hookup do
        other = [2, 0, 0, 0,
                 0, 2, 0, 0,
                 0, 0, 2, 0,
                 0, 0, 0, 2]

        topic.multiply_by! Ray::Matrix.new(other)
      end

      asserts(:content).equals((1..16).map { |n| n * 2 })
    end
  end

  asserts(:content=, (1..17).to_a).raises_kind_of ArgumentError
  asserts(:content=, (1..15).to_a).raises_kind_of ArgumentError
end

context "a matrix created with Matrix.[]" do
  setup { Ray::Matrix[*(1..16)] }
  asserts(:content).equals((1..16).to_a)
end

context "a translation matrix" do
  vector = Ray::Vector3[1, 2, 3]

  setup { Ray::Matrix.translation(vector) }
  asserts(:transform, -vector).equals Ray::Vector3[0, 0, 0]
end

context "a rotation matrix" do
  setup { Ray::Matrix.rotation(90, [0, 0, 1]) }

  asserts(:transform, [0, 0, 0]).almost_equals Ray::Vector3[0, 0, 0]
  asserts(:transform, [10, 0, 0]).almost_equals(Ray::Vector3[0, 10, 0], 1e-6)
end

context "a scaling matrix" do
  setup { Ray::Matrix.scale [10, 5, 1] }

  asserts(:transform, [0, 0, 0]).equals Ray::Vector3[0, 0, 0]
  asserts(:transform, [10, 10, 0]).equals Ray::Vector3[100, 50, 0]
end

context "an orthogonal projection" do
  setup { Ray::Matrix.orthogonal(0, 100, 100, 0, 1, -1) }

  asserts(:transform, [0, 0, 0]).equals Ray::Vector3[-1, 1, 0]
  asserts(:transform, [100, 100, 0.5]).
    almost_equals Ray::Vector3[1, -1, 0.5], 1e-6
end

context "a rotation then a translation" do
  setup { Ray::Matrix.rotation(-90, [0, 0, 1]).translate([10, 10, 0]) }
  asserts(:transform, [0, 0, 0]).equals Ray::Vector3[10, -10, 0]
end

context "a translation then a rotation" do
  setup { Ray::Matrix.translation([10, 10, 0]).rotate(-90, [0, 0, 1]) }
  asserts(:transform, [0, 0, 0]).equals Ray::Vector3[10, 10, 0]
end

context "any transformation" do
  setup { Ray::Matrix.rotation(90, [0, 0, 1]).scale([2, 3, 4]) }

  asserts "inverse matrix reverts transformations" do
    point = topic.transform [10, 20, 30]
    (topic.inverse.transform(point) - [10, 20, 30]).to_a.all? do |n|
      n.abs < 1e-5
    end
  end

  asserts(:untransform, [1, 2, 3]).equals { topic.inverse.transform [1, 2, 3] }
end

run_tests if __FILE__ == $0
