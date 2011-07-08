require File.expand_path(File.dirname(__FILE__)) + '/helpers.rb'

context "an int array" do
  setup { Ray::GL::IntArray.new }

  asserts(:[], 0).nil
  asserts(:size).equals 0

  context "with new elements" do
    hookup { topic << 3 << 0 << 1 }

    asserts(:[], 0).equals 3
    asserts(:[], 1).equals 0
    asserts(:[], 2).equals 1
    asserts(:[], 3).nil

    asserts(:size).equals 3
    asserts(:to_a).equals [3, 0, 1]

    context "one of which was changed" do
      hookup { topic[1] = 2 }

      asserts(:[], 0).equals 3
      asserts(:[], 1).equals 2
      asserts(:[], 2).equals 1
      asserts(:[], 3).nil

      asserts(:size).equals 3
      asserts(:to_a).equals [3, 2, 1]
    end

    context "but cleared" do
      hookup { topic.clear }

      asserts(:to_a).equals []
      asserts(:size).equals 0
    end

    context "copied" do
      setup { topic.dup }

      asserts(:size).equals 3
      asserts(:to_a).equals [3, 0, 1]
    end
  end

  context "resized with []=" do
    hookup { topic[1] = 5 }

    asserts(:[], 0).equals 0
    asserts(:[], 1).equals 5
    asserts(:[], 2).nil

    asserts(:size).equals 2
    asserts(:to_a).equals [0, 5]
  end
end

context "an int array created with values" do
  setup { Ray::GL::IntArray.new 0, 5, 4 }
  asserts(:to_a).equals [0, 5, 4]
end

run_tests if __FILE__ == $0
