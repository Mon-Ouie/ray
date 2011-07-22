require File.expand_path(File.dirname(__FILE__)) + '/helpers.rb'

class SomeEffect < Ray::Effect
  Code = %{
    vec4 effect(ray_some_effect args, vec4 color) {
      return color;
    }
  }

  Header = %{
    foo bar;
  }

  effect_name :some_effect
  attribute   :thing, "vec4"
  attribute   :array, "vec2[10]"

  def code
    Code
  end

  def header
    Header
  end

  def defaults
    {
      :thing     => [4, 3, 2, 1],
      "array[0]" => [10, 20]
    }
  end
end

context "an effect" do
  setup { SomeEffect.new }

  asserts(:name).equals :some_effect

  asserts(:header).equals SomeEffect::Header

  asserts(:struct).matches "struct ray_some_effect"
  asserts(:struct).matches "bool enabled;"
  asserts(:struct).matches "vec4 thing;"
  asserts(:struct).matches "vec2 array[10];"

  asserts(:code).equals SomeEffect::Code

  context "default elements" do
    setup do
      hash = {}
      topic.apply_defaults hash
      hash
    end

    asserts_topic.equals("some_effect.enabled"  => true,
                         "some_effect.thing"    => [4,   3, 2, 1],
                         "some_effect.array[0]" => [10, 20])
  end
end

run_tests if __FILE__ == $0
