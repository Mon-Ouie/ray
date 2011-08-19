require File.expand_path(File.dirname(__FILE__)) + '/helpers.rb'

context "an input" do
  setup { Ray::Input.new }

  denies(:holding?, Ray::Event::KeyA)
  denies(:holding?, Ray::Event::KeyNum0)

  asserts(:mouse_pos).equals Ray::Vector2[0, 0]

  context "with a new mouse position" do
    hookup { topic.mouse_pos = [10, 20] }
    asserts(:mouse_pos).equals Ray::Vector2[10, 20]
  end

  context "after pressing some keys" do
    hookup do
      topic.press Ray::Event::KeyA
      topic.press Ray::Event::KeyNum0
    end

    asserts(:holding?, Ray::Event::KeyA)
    asserts(:holding?, Ray::Event::KeyNum0)

    context "and releasing one" do
      hookup { topic.release Ray::Event::KeyA }

      denies(:holding?, Ray::Event::KeyA)
      asserts(:holding?, Ray::Event::KeyNum0)
    end
  end

  context "reset after several changes" do
    hookup do
      topic.mouse_pos = [4, 3]
      topic.press Ray::Event::KeyZ
      topic.press Ray::Event::KeyNum9

      topic.reset
    end

    asserts(:mouse_pos).equals Ray::Vector2[0, 0]

    denies(:holding?, Ray::Event::KeyZ)
    denies(:holding?, Ray::Event::KeyNum9)
  end
end

run_tests if __FILE__ == $0
