require File.expand_path(File.dirname(__FILE__) + '/../helpers.rb')

animation_context("a color variation", :color_variation, :from => Ray::Color.blue,
                  :to => Ray::Color.red) do
  setup { animation.start OpenStruct.new(:color => Ray::Color.green) }

  asserts_topic.kind_of Ray::Animation::ColorVariation
  asserts(:initial_value).equals Ray::Color.blue
  asserts(:variation).equals [255, 0, -255, 0]
  asserts(:attribute).equals :color
  asserts(:duration).equals 5

  context "reversed" do
    setup { -topic }

    asserts(:initial_value).equals Ray::Color.red
    asserts(:variation).equals [-255, 0, 255, 0]
    asserts(:attribute).equals :color
    asserts(:duration).equals 5
  end

  context "finished" do
    setup do
      topic = finished_animation.start(OpenStruct.new(:color => Ray::Color.green))
      topic.update
      topic
    end

    asserts("value of the target") { topic.target.color }.equals Ray::Color.red
  end
end

animation_context("a color variation using :of", :color_variation,
                  :of => [10, 50, 20, 30]) do
  setup { animation.start OpenStruct.new(:color => Ray::Color.green) }

  asserts_topic.kind_of Ray::Animation::ColorVariation
  asserts(:initial_value).nil
  asserts(:variation).equals [10, 50, 20, 30]
  asserts(:attribute).equals :color
  asserts(:duration).equals 5

  context "reversed" do
    setup { -topic }

    asserts(:initial_value).nil
    asserts(:variation).equals [-10, -50, -20, -30]
    asserts(:attribute).equals :color
    asserts(:duration).equals 5
  end

  context "finished" do
    setup do
      topic = finished_animation.start(OpenStruct.new(:color => Ray::Color.none))
      topic.update
      topic
    end

    asserts("value of the target") { topic.target.color }.
      equals Ray::Color.new(10, 50, 20, 30)
  end
end

run_tests if __FILE__ == $0
