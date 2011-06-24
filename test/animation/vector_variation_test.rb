require File.expand_path(File.dirname(__FILE__) + '/../helpers.rb')

animation_context("a vector2 variation", :vector_variation, :from => [10, 10],
                  :to => [20, 15], :attribute => :pos) do
  setup { animation.start OpenStruct.new(:pos => Ray::Vector2[5, 5]) }

  asserts_topic.kind_of Ray::Animation::VectorVariation
  asserts(:initial_value).equals Ray::Vector2[10, 10]
  asserts(:variation).equals Ray::Vector2[10, 5]
  asserts(:attribute).equals :pos
  asserts(:duration).equals 5

  context "reversed" do
    setup { -topic }

    asserts(:initial_value).equals Ray::Vector2[20, 15]
    asserts(:variation).equals Ray::Vector2[-10, -5]
    asserts(:attribute).equals :pos
    asserts(:duration).equals 5
  end

  context "finished" do
    setup do
      topic = finished_animation.start(OpenStruct.new(:pos => Ray::Vector2[5, 5]))
      topic.update
      topic
    end

    asserts("value of the target") { topic.target.pos }.equals Ray::Vector2[20, 15]
  end
end

animation_context("a vector2 variation using :of", :vector_variation,
                  :of => [10, 5], :attribute => :pos) do
  setup { animation.start OpenStruct.new(:pos => Ray::Vector2[5, 5]) }

  asserts_topic.kind_of Ray::Animation::VectorVariation
  asserts(:initial_value).nil
  asserts(:variation).equals Ray::Vector2[10, 5]
  asserts(:attribute).equals :pos
  asserts(:duration).equals 5

  context "reversed" do
    setup { -topic }

    asserts(:initial_value).nil
    asserts(:variation).equals Ray::Vector2[-10, -5]
    asserts(:attribute).equals :pos
    asserts(:duration).equals 5
  end

  context "finished" do
    setup do
      topic = finished_animation.start(OpenStruct.new(:pos => Ray::Vector2[5, 5]))
      topic.update
      topic
    end

    asserts("value of the target") { topic.target.pos }.equals Ray::Vector2[15, 10]
  end
end

animation_context("a vector3 variation", :vector_variation, :from => [10, 10, 5],
                  :to => [20, 15, 10], :attribute => :pos) do
  setup { animation.start OpenStruct.new(:pos => Ray::Vector3[5, 5, 0]) }

  asserts_topic.kind_of Ray::Animation::VectorVariation
  asserts(:initial_value).equals Ray::Vector3[10, 10, 5]
  asserts(:variation).equals Ray::Vector3[10, 5, 5]
  asserts(:attribute).equals :pos
  asserts(:duration).equals 5

  context "reversed" do
    setup { -topic }

    asserts(:initial_value).equals Ray::Vector3[20, 15, 10]
    asserts(:variation).equals Ray::Vector3[-10, -5, -5]
    asserts(:attribute).equals :pos
    asserts(:duration).equals 5
  end

  context "finished" do
    setup do
      topic = finished_animation.start(OpenStruct.new(:pos => Ray::Vector3[5, 5, 0]))
      topic.update
      topic
    end

    asserts("value of the target") { topic.target.pos }.equals Ray::Vector3[20, 15, 10]
  end
end

animation_context("a vector3 variation using :of", :vector_variation,
                  :of => [10, 5, 5], :attribute => :pos) do
  setup { animation.start OpenStruct.new(:pos => Ray::Vector3[5, 5, 0]) }

  asserts_topic.kind_of Ray::Animation::VectorVariation
  asserts(:initial_value).nil
  asserts(:variation).equals Ray::Vector3[10, 5, 5]
  asserts(:attribute).equals :pos
  asserts(:duration).equals 5

  context "reversed" do
    setup { -topic }

    asserts(:initial_value).nil
    asserts(:variation).equals Ray::Vector3[-10, -5, -5]
    asserts(:attribute).equals :pos
    asserts(:duration).equals 5
  end

  context "finished" do
    setup do
      topic = finished_animation.start(OpenStruct.new(:pos => Ray::Vector3[5, 5, 0]))
      topic.update
      topic
    end

    asserts("value of the target") { topic.target.pos }.equals Ray::Vector3[15, 10, 5]
  end
end

run_tests if __FILE__ == $0
