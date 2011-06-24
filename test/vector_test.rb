require File.expand_path(File.dirname(__FILE__)) + '/helpers.rb'

context "a vector2" do
  setup { Ray::Vector2[5, 15] }

  asserts(:x).equals(5)
  asserts(:y).equals(15)

  asserts(:w).equals(5)
  asserts(:h).equals(15)

  asserts(:+, [1, 2]).equals Ray::Vector2[6, 17]
  asserts(:-, [1, 2]).equals Ray::Vector2[4, 13]

  asserts(:*, 2).equals Ray::Vector2[10, 30]
  asserts(:*, [2, 1]).equals Ray::Vector2[10, 15]
  asserts(:/, [2, 1]).equals Ray::Vector2[2.5, 15]
  asserts(:dot, [2, 1]).equals 25

  asserts(:-@).equals Ray::Vector2[-5, -15]
  asserts(:+@).equals Ray::Vector2[+5, +15]

  asserts :inside?, Ray::Rect.new(0, 0, 20, 20)
  asserts :outside?, Ray::Rect.new(8, 0, 20, 20)

  asserts(:to_a).equals([5, 15])

  asserts(:length).almost_equals Math.sqrt(5 * 5 + 15 * 15)

  asserts(:dist, [5, 15]).equals 0
  asserts(:dist, [0, 15]).almost_equals 5

  asserts("equals to #to_a") { topic == [5, 15] }
  asserts("equals to the same vector") { topic == Ray::Vector2[5, 15] }
  asserts("equals to #dup") { topic == topic.dup }
  asserts("eql? to the same vector") { topic.eql? Ray::Vector2[5, 15] }
  asserts(:hash).equals { Ray::Vector2[5, 15].hash }

  denies(:==, [5, 15, 20])

  context "after changing x" do
    hookup { topic.x = 10 }

    asserts(:x).equals(10)
    asserts(:y).equals(15)

    asserts(:w).equals(10)
    asserts(:h).equals(15)
  end

  context "after changing y" do
    hookup { topic.y = 2 }

    asserts(:x).equals(5)
    asserts(:y).equals(2)

    asserts(:w).equals(5)
    asserts(:h).equals(2)
  end

  context "normalized" do
    hookup do
      @old_vector = topic.dup
      topic.normalize!
    end

    asserts(:length).almost_equals 1, 10e-6

    asserts("is a multiple of the old vector") do
      (topic.x / @old_vector.x - topic.y / @old_vector.y).abs < 10e-6
    end

    asserts_topic.equals { @old_vector.normalize }
  end
end

context "a vector3" do
  setup { Ray::Vector3[5, 15, 20] }

  asserts(:x).equals(5)
  asserts(:y).equals(15)
  asserts(:z).equals(20)

  asserts(:to_a).equals([5, 15, 20])

  asserts(:+, [1, 2, 3]).equals Ray::Vector3[6, 17, 23]
  asserts(:-, [1, 2, 3]).equals Ray::Vector3[4, 13, 17]

  asserts(:*, 2).equals Ray::Vector3[10, 30, 40]
  asserts(:*, [2, 0, 0.5]).equals Ray::Vector3[10, 0, 10]
  asserts(:/, [2, 1, 0.5]).equals Ray::Vector3[2.5, 15, 40]
  asserts(:dot, [2, 0, 0.5]).equals 20

  asserts(:-@).equals Ray::Vector3[-5, -15, -20]
  asserts(:+@).equals Ray::Vector3[+5, +15, +20]

  asserts(:length).almost_equals Math.sqrt(5 * 5 + 15 * 15 + 20 * 20)

  asserts(:dist, [5, 15, 20]).equals 0
  asserts(:dist, [5, 15, 0]).almost_equals 20

  asserts("equals to #to_a") { topic == [5, 15, 20] }
  asserts("equals to the same vector") { topic == Ray::Vector3[5, 15, 20] }
  asserts("equals to #dup") { topic == topic.dup }
  asserts("eql? to the same vector") { topic.eql? Ray::Vector3[5, 15, 20] }
  asserts(:hash).equals { Ray::Vector3[5, 15, 20].hash }

  denies(:==, [5, 15, 20, 30])

  context "after changing x" do
    hookup { topic.x = 10 }

    asserts(:x).equals(10)
    asserts(:y).equals(15)
    asserts(:z).equals(20)
  end

  context "after changing y" do
    hookup { topic.y = 2 }

    asserts(:x).equals(5)
    asserts(:y).equals(2)
    asserts(:z).equals(20)
  end

  context "after changing z" do
    hookup { topic.z = 3 }

    asserts(:x).equals(5)
    asserts(:y).equals(15)
    asserts(:z).equals(3)
  end

  context "normalized" do
    hookup do
      @old_vector = topic.dup
      topic.normalize!
    end

    asserts(:length).almost_equals 1, 10e-6

    asserts("is a multiple of the old vector") do
      val = topic.x / @old_vector.x

      (val - topic.y / @old_vector.y).abs < 10e-6 &&
        (val - topic.z / @old_vector.z).abs < 10e-6
    end

    asserts_topic.equals { @old_vector.normalize }
  end
end

run_tests if __FILE__ == $0
