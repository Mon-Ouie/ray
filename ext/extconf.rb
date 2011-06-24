require 'mkmf'

$CFLAGS  << " -Wextra -Wall -Wno-unused-parameter -std=gnu99 -g -O0 "
$LDFLAGS << " -L/usr/X11/lib "

$CFLAGS  << `freetype-config --cflags`.chomp
$LDFLAGS << `freetype-config --libs`.chomp

have_header "X11/extensions/Xrandr.h"
have_library "Xrandr"

deps = %w[X11 GL GLEW openal sndfile]

if deps.all? { |dep| have_library dep }
  create_makefile('ray_ext')
end

