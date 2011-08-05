require File.expand_path("helpers.rb", File.dirname(__FILE__))

MagicVertex = Ray::GL::Vertex.make [
  [:vertex,   "bar", :float],
  [:instance, "baz", :float, true],
]

context "a buffer" do
  setup { Ray::GL::Buffer.new :static, Ray::Vertex }

  asserts(:size).equals 256
  asserts(:[]=, 10, MagicVertex.new).raises_kind_of TypeError

  asserts(:[], 256).nil
  asserts(:[]=, 257, Ray::Vertex.new).raises_kind_of RangeError

  denies :has_instance?
  asserts(:instance_size).nil
  asserts(:resize_instance, 300).raises_kind_of RuntimeError
  asserts(:get_instance, 0).raises_kind_of RuntimeError
  asserts(:set_instance, 0, MagicVertex::Instance.new).
    raises_kind_of RuntimeError

  context "nth vertex after setting it" do
    setup do
      @buf = topic
      topic[34] = Ray::Vertex.new([10, 20], Ray::Color.red, [30, 40])
      topic[34]
    end

    asserts(:pos).equals Ray::Vector2[10, 20]
    asserts(:col).equals Ray::Color.red
    asserts(:tex).equals Ray::Vector2[30, 40]

    context "and resizing the buffer" do
      setup do
        @buf.resize 300
        @buf[34]
      end

      asserts(:pos).equals Ray::Vector2[10, 20]
      asserts(:col).equals Ray::Color.red
      asserts(:tex).equals Ray::Vector2[30, 40]
    end
  end
end

context "a buffer with per-instance data" do
  setup { Ray::GL::Buffer.new :static, MagicVertex }

  asserts :has_instance?
  asserts(:instance_size).equals 0
  asserts(:get_instance, 0).nil
  asserts(:set_instance, 0, MagicVertex::Instance.new).
    raises_kind_of RangeError

  context "resized" do
    hookup { topic.resize_instance 300 }
    asserts(:instance_size).equals 300

    context "nth instance" do
      setup { topic.set_instance 10, MagicVertex::Instance.new(42) }
      asserts(:instance).equals 42
    end
  end
end

run_tests if __FILE__ == $0
