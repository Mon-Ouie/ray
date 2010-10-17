# Hello world!
## Subclassing Ray::Game or Ray::Scene
    require 'ray'

    class HelloScene < Ray::Scene
      scene_name :hello
  
      def setup
        @font = font(path_of("VeraMono.ttf"), 12)
      end
  
      def render(win)
        @font.draw("Hello world!", :on => win, :at => [0, 0])
      end
    end

    class HelloWorld < Ray::Game
      def initialize
        super("Hello world!")
    
        HelloScene.bind(self)
        push_scene :hello
      end
  
      def register
        add_hook :quit, method(:exit!)
      end
    end

    HelloWorld.new.run
## DSL
    require 'ray'

    Ray::Game.new("Hello world!") do
      register do
        add_hook :quit, method(:exit!)
      end
  
      scene :hello do
        @font = font(path_of("VeraMono.ttf"), 12)
    
        render do |win|
          @font.draw("Hello world", :on => win, :at => [0, 0])
        end
      end
  
      push_scene :hello
    end
# Installation
Run the following command:

    gem install ray

This requires the SDL to be installed. SDL_TTF, SDL_image, SDL_gfx, and
SDL_mixer are also needed for several features, though you can install ray
without them.
# Features
* Hides the event loop where registred blocks or methods are run when something
  happens.
* Provides a cache for resources like images, fonts, sounds, ...
* Documented, using YARD. You can check the documentation on
  [rubydoc.info](http://rubydoc.info/github/Mon-Ouie/ray/master/frames)