require File.expand_path(File.dirname(__FILE__)) + '/helpers.rb'

context Ray::Sprite do
  img = Ray::Image.new [2, 2]
  setup { Ray::Sprite }

  asserts("doesn't copy the image when created") do
    topic.new(img).image == img
  end

  asserts("converts the argument of #initialize to an image") do
    image = Ray::ImageSet[path_of("aqua.bmp")]
    sprite = Ray::Sprite.new(path_of("aqua.bmp"))

    sprite.image == image
  end
end

context "a sprite" do
  img = Ray::Image.new [100, 150]
  setup { Ray::Sprite.new(img) }

  asserts(:rect).equals(Ray::Rect.new(0, 0, 100, 150))
  asserts(:to_rect).equals(Ray::Rect.new(0, 0, 100, 150))

  asserts(:sub_rect).equals(Ray::Rect.new(0, 0, 100, 150))

  asserts(:color).equals(Ray::Color.white)

  denies :x_flipped?
  denies :y_flipped?

  context "after changing drawn rect" do
    hookup { topic.sub_rect = [50, 30, 100, 120].to_rect }
    asserts(:sub_rect).equals(Ray::Rect.new(50, 30, 100, 120))
  end

  context "using a sprite sheet" do
    hookup { topic.sheet_size = [10, 10] }
    asserts(:sub_rect).equals(Ray::Rect.new(0, 0, 10, 15))

    context "after changing position in it" do
      hookup { topic.sheet_pos = [3, 5] }
      asserts(:sub_rect).equals(Ray::Rect.new(30, 75, 10, 15))
    end

    context "after setting position to a floating number" do
      hookup { topic.sheet_pos = [3.2, 5.9] }
      asserts(:sub_rect).equals(Ray::Rect.new(30, 75, 10, 15))
    end

    context "after setting position to an out of bounds numbers" do
      hookup { topic.sheet_pos = [10, 10] }
      asserts(:sub_rect).equals(Ray::Rect.new(0, 0, 10, 15))
    end

    context "after disabling it" do
      hookup { topic.disable_sprite_sheet }
      asserts(:sub_rect).equals(Ray::Rect.new(0, 0, 100, 150))
    end
  end

  context "after changing color" do
    hookup { topic.color = Ray::Color.red }
    asserts(:color).equals(Ray::Color.red)
  end

  context "after enabling flip x" do
    hookup { topic.flip_x = true }
    asserts :x_flipped?
  end

  context "after enabling flip y" do
    hookup { topic.flip_y = true }
    asserts :y_flipped?
  end

  context "copied after several changes" do
    setup do
      topic.flip_x = true
      topic.color  = Ray::Color.red
      topic.dup
    end

    asserts(:image).equals img

    asserts :x_flipped?
    denies  :y_flipped?

    asserts(:color).equals Ray::Color.red
  end
end

run_tests if __FILE__ == $0
