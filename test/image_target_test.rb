require File.expand_path(File.dirname(__FILE__)) + '/helpers.rb'

context "an image target" do
  img = Ray::Image.new [50, 50]
  setup { Ray::ImageTarget.new img }

  asserts(:size).equals Ray::Vector2[50, 50]

  context "after draw & update" do
    hookup do
      topic.clear Ray::Color.red
      topic.update
    end

    asserts("color of image") { img[0, 0] }.equals Ray::Color.red
  end
end


run_tests if __FILE__ == $0
