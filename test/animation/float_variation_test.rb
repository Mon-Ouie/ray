require File.expand_path(File.dirname(__FILE__) + '/../helpers.rb')

animation_context("a float variation", :float_variation, :from => 0, :to => 10,
                  :attribute => :float) do
  setup { animation.start OpenStruct.new(:float => 5) }

  asserts_topic.kind_of Ray::Animation::FloatVariation
  asserts(:initial_value).equals 0
  asserts(:variation).equals 10
  asserts(:attribute).equals :float
  asserts(:duration).equals 5

  context "reversed" do
    setup { reversed_animation }

    asserts(:target).nil
    asserts(:initial_value).equals 10
    asserts(:variation).equals(-10)
    asserts(:attribute).equals :float
    asserts(:duration).equals 5
 end

  context "finished" do
    setup do
      topic = finished_animation.start(OpenStruct.new(:float => 5))
      topic.update
      topic
    end

    asserts("value of the target") { topic.target.float }.equals 10
  end
end

animation_context("a float variation using :of", :float_variation, :of => 15,
                  :attribute => :float) do
  setup { animation.start OpenStruct.new(:float => 5) }

  asserts_topic.kind_of Ray::Animation::FloatVariation
  asserts(:initial_value).nil
  asserts(:variation).equals 15
  asserts(:attribute).equals :float
  asserts(:duration).equals 5

  context "reversed" do
    setup { -topic }

    asserts(:target).nil
    asserts(:initial_value).nil
    asserts(:variation).equals(-15)
    asserts(:attribute).equals :float
    asserts(:duration).equals 5
 end

  context "finished" do
    setup do
      topic = finished_animation.start(OpenStruct.new(:float => 5))
      topic.update
      topic
    end

    asserts("value of the target") { topic.target.float }.equals 20
  end
end

animation_context "a rotation", :rotation, :of => 20 do
  setup { animation.start OpenStruct.new(:angle => 5) }

  asserts_topic.kind_of Ray::Animation::FloatVariation
  asserts(:initial_value).nil
  asserts(:variation).equals 20
  asserts(:attribute).equals :angle
  asserts(:duration).equals 5
end

run_tests if __FILE__ == $0
