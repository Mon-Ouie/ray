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

    asserts(:view).equals Ray::View.new([320, 240], [640, 480], [0, 0, 0.5, 0.5])
    asserts(:clip).equals Ray::Rect[0, 0, 320, 240]

    asserts(:default_view).equals Ray::View.new([320, 240], [640, 480])
  end
end

run_tests if __FILE__ == $0
