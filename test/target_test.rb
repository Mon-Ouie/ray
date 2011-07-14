require File.expand_path(File.dirname(__FILE__)) + '/helpers.rb'

context "a target" do
  setup do
    win = Ray::Window.new
    win.open "test", [640, 480]
  end

  asserts(:view).equals Ray::View.new([320, 240], [640, 480])
  asserts(:default_view).equals Ray::View.new([320, 240], [640, 480])

  asserts(:shader).equals { topic.shader }

  asserts(:clip).equals Ray::Rect[0, 0, 640, 480]

  asserts(:viewport_for, [0, 0, 640, 480]).equals Ray::Rect[0, 0, 1, 1]
  asserts(:viewport_for, [320, 0, 320, 480]).equals Ray::Rect[0.5, 0, 0.5, 1]

  context "with a custom view" do
    hookup do
      view = topic.view
      view.viewport = Ray::Rect[0, 0, 0.5, 0.5]
      topic.view = view
    end

    asserts(:view).equals Ray::View.new([320, 240], [640, 480],
                                        [0, 0, 0.5, 0.5])
    asserts(:clip).equals Ray::Rect[0, 0, 320, 240]

    asserts(:default_view).equals Ray::View.new([320, 240], [640, 480])
  end

  context "cleared and updated" do
    hookup do
      # Why twice?
      # Because it seems glReadPixels *sometimes* uses the back buffer.
      2.times do
        topic.clear Ray::Color.green
        topic.update
      end
    end

    asserts(:[], 10, 30).equals Ray::Color.green

    context "and converted to an image" do
      setup { topic.to_image }

      asserts(:size).equals Ray::Vector2[640, 480]
      asserts(:[], 0, 0).equals Ray::Color.green
    end

    context "converted to an image created from a part of it" do
      setup { topic.rect [10, 20, 30, 40] }

      asserts(:size).equals Ray::Vector2[30, 40]
      asserts(:[], 0, 0).equals Ray::Color.green
    end
  end
end

run_tests if __FILE__ == $0
