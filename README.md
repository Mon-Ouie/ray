What is Ray?
============

Ray is a library than can create windows, play music, and draw 2D graphics (or
not too complex 3D by doing just a bit more work, thanks to OpenGL). It is meant
to be easy and fun to use (à la Shoes), and still flexible and powerful.

Tutorial & Installation
=======================

See [on this page](http://mon-ouie.github.com/projects/ray.html). Also notice
there's an IRC channel on freenode: #ray.

Features
========

Fun DSL
-------

```ruby
require 'ray'

Ray.game "Hello" do
  register { add_hook :quit, method(:exit!) }

  scene :hello do
    @text = text "Hello world!", :size => 30, :angle => 30, :at => [50, 50]
    render { |win| win.draw @text }
  end

  scenes << :hello
end
```

Flexible for more complex games
-------------------------------

```ruby
require 'ray'

class TitleScene < Ray::Scene
  scene_name :title

  def setup
    # setup resources
  end

  def register
    # register for events
  end

  def render(win)
    # draw!
  end

  def clean_up
    # perform cleanup
  end
end

class GameScene < Ray::Scene
  scene_name :game

  # same stuff
end

# ...

class Game < Ray::Game
  def initialize
    super "Awesome Game"

    TitleScene.bind(self)
    GameScene.bind(self)
    # ...

    scenes << :title
  end
end
```

Drawable API
------------

Ray has a drawable class that specifies a common interface to all the drawable
objects – how to perform transformations to them and how to draw them.

```ruby
 obj = AnyDrawable.new

 window.draw obj

 obj.pos    = [0, 2]   # set position
 obj.angle  = 40       # rotation
 obj.origin = [20, 20] # just sets the origin of transformations
 # ...

 # You can even have a completely custom transformation matrix:
 obj.matrix = Ray::Matrix.translation [2, 3, 4]
```

Off-screen rendering
--------------------

When rendering to a window is not enough, you can render on an image just fine:

```ruby
Ray::ImageTarget.new some_image do |target|
  target.clear Ray::Color.red
  target.draw Ray::Polygon.circle([50, 50], 10, Ray::Color.green)
  target.update
end
```

OpenGL integration
------------------

Ray uses OpenGL, and provides some classes and methods to allow using it from
Ray. In fact, ``Ray::Drawable`` can simply be used for OpenGL rendering:

```ruby
# Ray has a more advanced sprite class, of course!
class CustomSprite < Ray::Drawable
  def initialize(image)
    super() # very important: creating the actual drawable

    # Ray allocates a VBO to store your vertices.
    # You could just use your own OpenGL binding to call glBegin and glEnd
    # if you don't want to use it.
    self.vertex_count = 4

    # Tells Ray to enable texturing for this drawable
    self.textured = true

    @image = image
  end

  # return an array of vertices
  def fill_vertices
    rect = @img.tex_rect [0, 0, @img.w, @img.h]

    [
     Ray::Vertex.new([0,      0],      Ray::Color.white, rect.top_left),
     Ray::Vertex.new([@img.w, 0],      Ray::Color.white, rect.top_right),
     Ray::Vertex.new([0,      @img.h], Ray::Color.white, rect.bottom_left),
     Ray::Vertex.new([@img.w, @img.h], Ray::Color.white, rect.bottom_right),
    ]
  end

  # The index parameter is there in case you'd want to use
  # draw_elements. You can fill indices by defining a fill_indices method
  # and setting index_count.
  def render(first, index)
    @image.bind

    # Some low level OpenGL calls are available
    Ray::GL.draw_arrays :triangle_strip, first, 4
  end
end
```

You can also create and use shaders from Ruby:

```ruby
shader = Ray::Shader.new :vertex => "vertex_shader.glsl",
                         :frag   => "frag_shader.glsl"
drawable.shader = shader

# You can't assign an image to a render target, but you can recompile it:
window.shader.compile :vertex => "vertex.glsl", :frag => "frag.glsl"
```

3D rendering
------------

3D rendering is a very cool thing! Even if Ray's graphics module only uses 2D, a
3D API can be created with it. You can just use ``Ray::Drawable`` again, but you
will ned to specify the layout of your vertices and to write your own shaders
(the default shaders are only designed for 2D). You will probably need a custom
projection matrix too.

```ruby
class Cube < Ray::Drawable
  include Ray::GL

  Vertex = Ray::GL::Vertex.make [
    [:pos, "in_Position", :vector3],
    [:col, "in_Color",    :color]
  ]

  def initialize
    super Vertex
    # ...
  end

  # ...
end

# ...

# Tell the shader what vertex layout to use.
window.shader.apply_vertex Cube::Vertex
window.shader.compile :vertex => "vertex.glsl", :frag => "frag.glsl"

# Ray::Matrix can create 3D transformation and projection matrices!
window.view = Ray::View.new Ray::Matrix.perspective(90, 1, 1, 100)
```

Audio playback
--------------

Ray can play short sounds right away and stream longer ones — it uses OpenAL for
this. 3D audio effects can be added as well.

```ruby
@sound = sound "test.wav"
@music = music "test.ogg"

@sound.pause
@music.play

@music.pause

@music.volume = 80
@music.pitch  = 0.9
@music.pos    = [10, 20, 30]

@music.play
```

Testing
-------

Ray's events can be faked so that you can simulate user input in your tests:

```ruby
require 'awesome_scene'

describe AwesomeScene do
  before :each do
    @game = AwesomeGame.new
    @scene = @game.registered_scene(:awesome_scene)

    @scene.register
    @scene.setup
  end

  it "has a cursor at (0, 0)" do
    @scene.cursor.pos.should == [0, 0]
  end

  it "moves its cursor after the mouse moved" do
    @game.raise_event :mouse_motion, Ray::Vector2[100, 100]
    @game.event_runner.run

    @scene.cursor.pos.should == [100, 100]
  end

  it "draws its cursor" do
    @scene.window.should_receive(:draw, @scene.cursor)
    @scene.render @scene.window
  end

  after :each do
    @scene.clean_up
  end
end
```

Animations
----------

You can animate the fact the state of an object is changing (its position, etc.)
using Ray's animation objects:

```ruby
animations << translation(:from => [0, 0], :to => [100, 100],
                          :duration => 4).start(@some_drawable)
```

(They can really be used to animate any change, not just those that are visible,
and not just those applied to a drawable.)

