require File.expand_path("helpers.rb", File.dirname(__FILE__))

context "an index buffer" do
  setup { Ray::GL::IndexBuffer.new :static }

  asserts(:size).equals 256

  asserts(:[], 256).nil
  asserts(:[]=, 257, 3).raises_kind_of RangeError

  context "after changing an element" do
    hookup { topic[34] = 30 }
    asserts(:[], 34).equals 30

    context "and resizing the buffer" do
      hookup { topic.resize 300 }
      asserts(:[], 34).equals 30
    end
  end
end

run_tests if __FILE__ == $0
