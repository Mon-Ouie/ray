require 'mkmf'

$CFLAGS  << " -Wextra -Wall -Wno-unused-parameter -std=gnu99 -g"

unless RUBY_PLATFORM =~ /mingw/
  $CFLAGS  << " " << `freetype-config --cflags`.chomp
  $LDFLAGS << " " << `freetype-config --libs`.chomp
end

if RUBY_PLATFORM =~ /darwin/
  $CFLAGS  << " -I/usr/X11/include -DSAY_OSX -x objective-c"
  $LDFLAGS << " -L/usr/X11/lib -framework OpenAL -framework OpenGL"
  $LDFLAGS << " -framework Cocoa"

  deps = %w[GLEW sndfile]

  if deps.all? { |dep| have_library dep }
    create_makefile 'ray_ext'
  end
elsif RUBY_PLATFORM =~ /mingw/
  def vendor_path(file)
    File.join(File.dirname(__FILE__), "vendor", file)
  end

  def vendor(file)
    '"' + vendor_path(file) + '"'
  end

  def rel(file)
    File.join(File.dirname(__FILE__), file)
  end

  require 'fileutils'
  include FileUtils

  cp vendor_path("openal/Win32/soft_oal.dll"), rel("OpenAL32.dll")
  cp vendor_path("freetype/bin/freetype6.dll"), rel("freetype6.dll")
  cp vendor_path("freetype/bin/zlib1.dll"), rel("zlib1.dll")
  cp vendor_path("glew/lib/glew32.dll"), rel("glew32.dll")
  cp vendor_path("sndfile/bin/libsndfile-1.dll"), rel("libsndfile-1.dll")

  $CFLAGS << " -DSAY_WIN"

  # Windows API
  have_library "gdi32"

  # OpenGL and GLEW
  $CFLAGS  << " -I#{vendor("glew/include")}"
  $LDFLAGS << " -L#{vendor("glew/lib")}"

  have_library "opengl32"
  have_library "glew32"

  # Freetype
  $CFLAGS  << " -I#{vendor("freetype/include")}"
  $CFLAGS  << " -I#{vendor("freetype/include/freetype2")}"
  $LDFLAGS << " -L#{vendor("freetype/lib")}"

  have_library "libfreetype"

  # OpenAL
  $CFLAGS  << " -I#{vendor("openal/include")}"
  $LDFLAGS << " -L#{vendor("openal/lib")}"

  have_library "OpenAL32"

  # libsndfile
  $CFLAGS  << " -I#{vendor("sndfile/include")}"
  $LDFLAGS << " -L#{vendor("sndfile/lib")}"

  have_library "libsndfile-1"

  create_makefile 'ray_ext'
else
  $CFLAGS << " -DSAY_X11"

  have_header "X11/extensions/Xrandr.h"
  have_library "Xrandr"

  deps = %w[X11 GL GLEW openal sndfile]

  if deps.all? { |dep| have_library dep }
    create_makefile 'ray_ext'
  end
end

