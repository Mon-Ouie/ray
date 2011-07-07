require 'mkmf'

$CFLAGS  << " -Wextra -Wall -Wno-unused-parameter -std=gnu99 -g -O0"

$CFLAGS  << " " << `freetype-config --cflags`.chomp
$LDFLAGS << " " << `freetype-config --libs`.chomp

if RUBY_PLATFORM =~ /darwin/
  $CFLAGS  << " -I/usr/X11/include -DSAY_OSX -x objective-c"
  $LDFLAGS << " -L/usr/X11/lib -framework OpenAL -framework OpenGL"
  $LDFLAGS << " -framework Cocoa"

  deps = %w[GLEW sndfile]

  if deps.all? { |dep| have_library dep }
    create_makefile('ray_ext')
  end
else
  $CFLAGS << " -DSAY_X11"

  have_header "X11/extensions/Xrandr.h"
  have_library "Xrandr"

  deps = %w[X11 GL GLEW openal sndfile]

  if deps.all? { |dep| have_library dep }
    create_makefile('ray_ext')
  end
end

