require File.expand_path(File.dirname(__FILE__)) + '/helpers.rb'

context Ray::Image do
  setup { Ray::Image }

  asserts(:new, path_of("pop.wav")).raises_kind_of RuntimeError
  asserts(:new, path_of("thing.png")).raises_kind_of RuntimeError

  asserts(:new, StringIO.new(File.read(path_of("pop.wav")))).
    raises_kind_of RuntimeError
end

context "an image loaded from a file" do
  setup { Ray::Image.new path_of("sprite.png") }

  asserts(:width).equals 128
  asserts(:height).equals 192
  asserts(:size).equals Ray::Vector2[128, 192]

  asserts(:[], 128, 100).nil
  asserts(:[], 100, 192).nil

  asserts(:[]=, 128, 100, Ray::Color.green).raises_kind_of RangeError
  asserts(:[]=, 100, 192, Ray::Color.green).raises_kind_of RangeError

  context "after changing a pixel" do
    hookup { topic[0, 10] = Ray::Color.green }
    asserts(:[], 0, 10).equals Ray::Color.green
    denies(:[], 1, 10).equals Ray::Color.green
  end
end

context "an image loaded from an IO" do
  setup { open(path_of("sprite.png")) { |io| Ray::Image.new(io) } }

  asserts(:width).equals 128
  asserts(:height).equals 192
  asserts(:size).equals Ray::Vector2[128, 192]
end

context "an image created from a size" do
  setup { Ray::Image.new [64, 128] }

  asserts(:width).equals 64
  asserts(:height).equals 128
  asserts(:size).equals Ray::Vector2[64, 128]

  asserts(:tex_rect, [0, 0, 64, 128]).equals Ray::Rect[0, 0, 1, 1]
  asserts(:tex_rect, [0, 0, 32, 128]).equals Ray::Rect[0, 0, 0.5, 1]
  asserts(:tex_rect, [0, 0, 64, 64]).equals Ray::Rect[0, 0, 1, 0.5]
  asserts(:tex_rect, [32, 32, 32, 32]).equals Ray::Rect[0.5, 0.25, 0.5, 0.25]

  denies :smooth?

  context "after enabling smoothing" do
    hookup { topic.smooth = true }
    asserts :smooth?
  end
end

context "an image copy" do
  setup do
    img = Ray::Image.new [2, 2]
    img[0, 0] = Ray::Color.red
    img[0, 1] = Ray::Color.green
    img[1, 0] = Ray::Color.blue
    img[1, 1] = Ray::Color.white

    img.dup
  end

  asserts(:size).equals Ray::Vector2[2, 2]

  asserts(:[], 0, 0).equals Ray::Color.red
  asserts(:[], 0, 1).equals Ray::Color.green
  asserts(:[], 1, 0).equals Ray::Color.blue
  asserts(:[], 1, 1).equals Ray::Color.white

  context "saved and loaded again" do
    path = path_of("test_save.png")

    setup do
      topic.write path
      Ray::Image.new path
    end

    asserts(:size).equals Ray::Vector2[2, 2]

    asserts(:[], 0, 0).equals Ray::Color.red
    asserts(:[], 0, 1).equals Ray::Color.green
    asserts(:[], 1, 0).equals Ray::Color.blue
    asserts(:[], 1, 1).equals Ray::Color.white

    teardown do
      File.delete path if File.exist? path
    end
  end
end

run_tests if __FILE__ == $0
