# -*- coding: utf-8 -*-
require File.expand_path(File.dirname(__FILE__)) + '/helpers.rb'

context "a text" do
  setup { Ray::Text.new "Hello world!" }

  asserts(:string).equals "Hello world!"

  asserts(:style).equals Ray::Text::Normal
  asserts(:size).equals 12
  asserts(:color).equals Ray::Color.white

  asserts(:auto_center).nil

  context "after changing style" do
    hookup { topic.style = [:bold, :italic] }
    asserts(:style).equals Ray::Text::Bold | Ray::Text::Italic
  end

  context "after changing string" do
    hookup do
      topic.string = "héllo"
    end

    asserts(:encoding).matches(/utf-8/i)
    asserts(:string).equals "héllo"

    context "copied" do
      setup do
        topic.color = Ray::Color.red
        topic.dup
      end

      asserts(:style).equals Ray::Text::Normal
      asserts(:size).equals 12
      asserts(:color).equals Ray::Color.red

      asserts(:encoding).matches(/utf-8/i)
      asserts(:string).equals "héllo"
    end
  end

  context "with auto centering" do
    hookup { topic.auto_center = [0.5, 1.0] }

    asserts(:auto_center).equals Ray::Vector2[0.5, 1.0]
    asserts(:origin).equals { topic.rect.size * [0.5, 1.0] }

    context "changed" do
      hookup { topic.string = "Hello dear world!" }
      asserts(:origin).equals { topic.rect.size * [0.5, 1.0] }
    end

    context "disabled" do
      hookup { topic.auto_center = nil }

      asserts(:auto_center).nil
      asserts(:origin).equals { topic.rect.size * [0.5, 1.0] }

      context "changed" do
        hookup { topic.string = "Hello great world!" }
        denies(:origin).equals { topic.rect.size * [0.5, 1.0] }
      end
    end
  end

  context "after changing character size" do
    hookup { topic.size = 30 }
    asserts(:size).equals 30
  end

  context "rect with another position and scale" do
    hookup do
      topic.pos   = [10, 20]
      topic.scale = [2, 10]
    end

    asserts("position of the rect") { topic.rect.pos }.equals Ray::Vector2[10, 20]
    asserts("size / size without scale") {
      scaled_size = topic.rect.size

      topic.scale = [1, 1]
      size = topic.rect.size
      topic.scale = [2, 10]

      scaled_size / size
    }.equals Ray::Vector2[2, 10]
  end

  context "after changing the color" do
    hookup { topic.color = Ray::Color.red }
    asserts(:color).equals Ray::Color.red
  end
end

run_tests if __FILE__ == $0
