require File.expand_path(File.dirname(__FILE__)) + '/helpers.rb'

context "a pixel bus" do
  img = Ray::Image.new [3, 2]
  img[0, 0] = Ray::Color.red
  img[0, 1] = Ray::Color.green
  img[1, 0] = Ray::Color.blue
  img[1, 1] = Ray::Color.white
  img[2, 0] = Ray::Color.cyan
  img[2, 1] = Ray::Color.yellow
  pixels = img.to_a

  setup { Ray::PixelBus.new }

  asserts(:size).equals 1024

  context "resized" do
    hookup { topic.resize! 500 }
    asserts(:size).equals 500
  end

  context "a copied image" do
    setup { topic.copy(img) }
    asserts(:to_a).equals pixels
  end
end

run_tests if __FILE__ == $0
