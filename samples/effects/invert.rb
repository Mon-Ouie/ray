$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../lib")
$:.unshift File.expand_path(File.dirname(__FILE__) + "/../../ext")

def path_of(res)
  File.expand_path(File.dirname(__FILE__) + "/../../test/res/#{res}")
end

require 'ray'

Ray.game "grayscale" do
  register { add_hook :quit, method(:exit!) }

  scene :shader do
    @obj = sprite path_of("sprite.png")

    effect_generator do |gen|
      gen << color_inversion << grayscale
      gen.build window.shader
    end

    @states = Hash.new(true)

    {
      :color_inversion => :i,
      :grayscale       => :g
    }.each do |state, key_name|
      on :key_press, key(key_name) do
        @states[state] = !@states[state]
        window.shader["#{state}.enabled"] = @states[state]
      end
    end

    @inverted = true

    on :key_press, key(:i) do

    end

    render do |win|
      win.draw @obj
    end
  end

  scenes << :shader
end
