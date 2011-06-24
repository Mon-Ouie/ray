require File.expand_path(File.dirname(__FILE__) + '/../helpers.rb')

context "an animation combination" do
  setup do
    obj = Object.new
    obj.extend Ray::Helper

    @target = OpenStruct.new(:color => Ray::Color.black,
                             :pos   => Ray::Vector2[50, 50])

    @first  = obj.translation(:of => [10, 10], :duration => 15)
    @second = obj.color_variation(:of => [10, 20, 30, 0], :duration => 10)

    obj.animation_combination(@first, @second).start(@target)
  end

  asserts_topic.kind_of Ray::Animation::Combination

  asserts("first element's target")  { @first.target  }.equals { @target }
  asserts("second element's target") { @second.target }.equals { @target }

  asserts(:duration).equals 15

  context "updated" do
    hookup do
      stub(@second).update

      topic.update
    end

    asserts("each element") { @second }.received(:update)
  end
end

run_tests if __FILE__ == $0
