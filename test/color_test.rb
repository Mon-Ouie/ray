require File.expand_path(File.dirname(__FILE__)) + '/helpers.rb'

context "a color" do
  setup { Ray::Color.new(10, 10, 10, 10) }

  asserts(:*, Ray::Color.red).equals Ray::Color.new(10, 0, 0, 10)
  asserts(:+, Ray::Color.new(20, 10, 4, 3)).equals Ray::Color.new(30, 20, 14, 13)

  asserts_topic.equals Ray::Color.new(10, 10, 10, 10)
  denies_topic.equals Ray::Color.new(20, 10, 10, 10)

  [:red, :green, :blue, :alpha].each do |comp|
    context "after having changed #{comp} to 30" do
      hookup { topic.send("#{comp}=", 30) }

      asserts(comp).equals(30)

      asserts "other components keep their values" do
        [:red, :green, :blue, :alpha].all? do |other|
          other == comp || topic.send(other) == 10
        end
      end
    end
  end

  context "created with 3 integers" do
    setup { Ray::Color.new(10, 20, 30) }

    asserts(:r).equals 10
    asserts(:g).equals 20
    asserts(:b).equals 30
    asserts(:a).equals 255
  end

  context "created with 4 integers" do
    setup { Ray::Color.new(10, 20, 30, 127) }

    asserts(:r).equals 10
    asserts(:g).equals 20
    asserts(:b).equals 30
    asserts(:a).equals 127
  end

  context "with a component set to more than 255" do
    hookup { topic.r = 300 }
    asserts(:r).equals 255
  end

  context "with a component set to less than 0" do
    hookup { topic.r = -100 }
    asserts(:r).equals 0
  end

  context "copied from another color" do
    setup do
      @old_topic = topic
      topic.dup
    end

    asserts(:r).equals(10)
    asserts(:g).equals(10)
    asserts(:b).equals(10)
    asserts(:g).equals(10)

    denies "is the same object as the original" do
      topic.equal? @old_topic
    end

    asserts "is eql? to the original" do
      topic.eql? @old_topic
    end

    asserts(:hash).equals { @old_topic.hash }
  end
end

run_tests if __FILE__ == $0
