require File.expand_path(File.dirname(__FILE__)) + '/helpers.rb'

Ray::Shader.use_old!

context "a shader" do
  setup { Ray::Shader.new }

  context "compiled" do
    asserts("with syntax error") {
      topic.compile(:frag => StringIO.new("foo"))
    }.raises_kind_of Ray::Shader::CompileError

    asserts("from a nonexistent file") {
      topic.compile(:vertex => path_of("foo"))
    }.raises_kind_of SystemCallError

    denies("from an existent file") {
      topic.compile(:vertex => path_of("vert.c"), :frag => path_of("frag.c"))
    }.raises_kind_of Exception

    asserts("from a non-existant file") {
      topic.compile(:vertex => "foo")
    }.raises_kind_of SystemCallError

    asserts("with an undefined main function") {
      topic.compile(:vertex => StringIO.new("void foo() {}"))
    }.raises_kind_of Ray::Shader::LinkError
  end

  context "with some settings" do
    hookup do
      topic.compile :vertex => StringIO.new(<<-vert), :frag => StringIO.new(<<-frag)
        #version 110
        uniform vec4 pos;
        void main() {
          gl_Position = pos;
        }
      vert
        #version 110
        uniform vec4 color;
        void main() {
          gl_FragColor = color;
        }
      frag
    end

    asserts(:locate, :color)
    asserts(:locate, :pos)
    denies(:locate, :foo)

    denies(:[]=, :pos,   [0.5, 0.5, 0.5, 1]).raises_kind_of Exception
    denies(:[]=, :color, [1, 0, 0, 1]).raises_kind_of Exception

    asserts(:[]=, :foo, 3).raises_kind_of Ray::Shader::NoUniformError
  end
end

run_tests if __FILE__ == $0
