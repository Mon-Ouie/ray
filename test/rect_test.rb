require File.expand_path(File.dirname(__FILE__)) + '/helpers.rb'

context "a rect" do
  setup { Ray::Rect.new(10, 15, 20, 30) }

  asserts(:size).equals Ray::Vector2[20, 30]
  asserts(:pos).equals  Ray::Vector2[10, 15]

  [:x, :y, :w, :h].each do |param|
    context "after having changed #{param}" do
      setup do |rect|
        rect = Ray::Rect.new(10, 10, 10, 10)
        rect.send("#{param}=", 5)
        rect
      end

      asserts(param).equals(5)

      asserts "the others components keep their values" do
        [:x, :y, :w, :h].all? do |other|
          other == param || topic.send(other) == 10
        end
      end
    end
  end

  context "copied from another rect" do
    setup do
      @old_topic = topic
      topic.dup
    end

    asserts_topic.equals { @old_topic }

    asserts "is eql? to the original object" do
      topic.eql? @old_topic
    end

    asserts(:hash).equals { @old_topic.hash }
  end

  context "created with two integers" do
    setup { Ray::Rect.new(30, 35) }

    asserts(:w).equals(0)
    asserts(:h).equals(0)

    asserts(:x).equals(30)
    asserts(:y).equals(35)
  end

  asserts("creating a rect with a hash without x or y") {
    Ray::Rect.new(:x => 10)
  }.raises_kind_of ArgumentError

  asserts("creting a rect with a hash with width but not height") {
    Ray::Rect.new(:x => 10, :y => 200, :width => 300)
  }.raises_kind_of ArgumentError

  asserts("creating a rect with a hash with a size but without position") {
    Ray::Rect.new(:width => 100, :height => 100)
  }.raises_kind_of ArgumentError

  asserts "inside itself" do
    topic.inside? Ray::Rect.new(10, 15, 20, 30)
  end

  asserts "inside a rect inside itself" do
    topic.inside? Ray::Rect.new(8, 13, 30, 40)
  end

  asserts "not inside a rect only colliding with it" do
    !Ray::Rect.new(5, 10, 20, 20).inside?(topic) &&
      !Ray::Rect.new(11, 16, 40, 50).inside?(topic)
  end

  asserts "not inside a rect outside the receiver" do
    !Ray::Rect.new(1, 2, 3, 5).inside?(topic) &&
      !Ray::Rect.new(100, 150, 350, 450).inside?(topic)
  end

  asserts "not outside itself" do
    not topic.outside? Ray::Rect.new(10, 15, 20, 30)
  end

  asserts "outside a rect outside itself" do
    Ray::Rect.new(1, 2, 3, 5).outside?(topic) &&
      Ray::Rect.new(100, 150, 350, 450).outside?(topic)
  end

  asserts "not outside a rect inside itself" do
    not Ray::Rect.new(13, 17, 10, 20).outside?(topic)
  end

  asserts "not outside a rect colliding with itself" do
    !Ray::Rect.new(5, 10, 20, 20).outside?(topic) &&
      !Ray::Rect.new(11, 16, 40, 50).outside?(topic)
  end

  asserts "collides with itself" do
    topic.collide? Ray::Rect.new(10, 15, 20, 30)
  end

  asserts "doesn't collide with a rect outside itself" do
    !Ray::Rect.new(1, 2, 3, 5).collide?(topic) &&
      !Ray::Rect.new(100, 150, 350, 450).collide?(topic)
  end

  asserts "collides with a rect inside itself" do
    Ray::Rect.new(13, 17, 10, 20).collide?(topic)
  end

  asserts "collides with rects colliding with itself" do
    Ray::Rect.new(5, 10, 20, 20).collide?(topic) &&
      Ray::Rect.new(11, 16, 40, 50).collide?(topic)
  end

  asserts "collides with a rect it is inside of" do
    topic.collide? Ray::Rect[11, 16, 1, 1]
  end
end

run_tests if __FILE__ == $0
