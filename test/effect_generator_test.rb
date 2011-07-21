require File.expand_path(File.dirname(__FILE__)) + '/helpers.rb'

context "an effect generator" do
  setup do
    gen = Ray::Effect::Generator.new
    gen << (@effect = Ray::Effect::Grayscale.new([3, 5, 2]))
  end

  asserts(:effects).equals { [@effect] }

  asserts(:version).equals 110

  asserts(:input).matches "vec4 var_Color"
  asserts(:input).matches "vec2 var_TexCoord"
  asserts(:input).matches "varying "
  denies(:input).matches  "in "

  asserts(:uniforms).matches "uniform sampler2D in_Texture"
  asserts(:uniforms).matches "uniform bool in_TextureEnabled"

  context "code" do
    setup do
      @struct = @effect.struct
      @code   = @effect.code
      stub(@effect).header { "HEADER" }
      topic.code
    end

    asserts_topic.matches "gl_FragColor"
    denies_topic.matches  "out_FragColor"

    asserts_topic.matches "uniform ray_grayscale grayscale;"

    asserts("headers are before structs") {
      topic.index("HEADER") < topic.index(@struct)
    }

    asserts("structs are before functions") {
      topic.index(@struct) < topic.index(@code)
    }

    asserts_topic.matches "if (grayscale.enabled)"
  end

  context "built shader" do
    setup do
      shader = Ray::Shader.new

      proxy(shader).compile
      proxy(shader).__send__(:[]=)

      topic.build shader

      shader
    end

    asserts_topic.received(:compile, is_a(Hash))
    asserts_topic.received(:[]=, "grayscale.enabled", true)
    asserts_topic.received(:[]=, "grayscale.ratio", [3, 5, 2])
  end
end

run_tests if __FILE__ == $0
