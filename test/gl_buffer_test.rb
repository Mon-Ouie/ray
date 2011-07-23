require File.expand_path("helpers.rb", File.dirname(__FILE__))

MagicVertex = Ray::GL::Vertex.make [[:foo, "bar", :float]]

context "a buffer" do
  setup { Ray::GL::Buffer.new :static, Ray::Vertex }

  asserts(:size).equals 256
  asserts(:[]=, 10, MagicVertex.new).raises_kind_of TypeError

  asserts(:[], 256).nil
  asserts(:[]=, 257, Ray::Vertex.new).raises_kind_of RangeError

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

run_tests if __FILE__ == $0
