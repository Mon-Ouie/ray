$:.unshift File.expand_path(File.dirname(__FILE__) + "/../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../ext")

require 'ray'

class FakeOutput
  def initialize
    @text = Ray::Text.new "", :size => 12
  end

  def puts(string)
    @text.string = @text.string << "#{string}\n"
  end

  def write(string)
    @text.string = @text.string << string
  end

  attr_reader :text
end

$stdout = FakeOutput.new

Ray.game "run_scene" do
  register do
    add_hook :quit, method(:exit!)

    on :key_press, key(:up) do
      puts "#{self} knows you pressed up"
    end
  end

  scene :sec do
    on :key_press, key(:down) do
      puts "#{self} knows you pressed down"
    end

    on :key_press, key(:left) do
      pop_scene
    end

    puts "In scene :sec"

    render do |win|
      win.draw $stdout.text
    end
  end

  scene :first do
    on :key_press, key(:left) do
      puts "#{self} knows you pressed left"
    end

    on :key_press, key(:right) do
      run_scene :sec
      puts "Back to scene :first"
    end

    puts "In scene :first"

    render do |win|
      win.draw $stdout.text
    end
  end

  push_scene :first
end
