require File.expand_path(File.dirname(__FILE__)) + '/helpers.rb'

context Ray::Font do
  setup { Ray::Font }

  denies("creating a font from a file") {
    topic.new(path_of("VeraMono.ttf"))
  }.raises_kind_of RuntimeError

  asserts("creating a font from a non-existing file") {
    topic.new(path_of("doesnt_exist.ttf"))
  }.raises_kind_of RuntimeError

  denies("creating a font from an IO object") {
    open(path_of("VeraMono.ttf")) { |io| topic.new(io) }
  }.raises_kind_of RuntimeError

  asserts("creating a font from an invalid IO object") {
    open(path_of("aqua.png")) { |io| topic.new(io) }
  }.raises_kind_of RuntimeError
end

run_tests if __FILE__ == $0
