require 'mkmf'

def find_macosx_framework(name)
  paths = [
           "/opt/local/Library/Frameworks",
           "/Library/Frameworks",
           "/Network/Library/Frameworks",
           "/System/Library/Frameworks"
          ]
  paths.reverse!

  paths.each do |dir|
    dir = File.join(dir, "#{name}.framework")
    next unless File.directory? dir

    return dir
  end

  nil
end

def have_framework(name)
  print "checking for framweork #{name}... "
  ret = find_macosx_framework(name)

  if ret
    puts "yes"

    $LDFLAGS << " -F#{File.dirname(ret)}"
    $LDFLAGS << " -framework #{name}"
    $CFLAGS  << " -I#{File.join(ret, "Headers")}"
    $CFLAGS  << " -DHAVE_#{name.upcase}"
  else
    puts "no"
  end
  ret
end

def have_sdl_ext(name, header)
  if res = have_framework(name)
    ending = header.upcase.tr('/', '_').gsub(/\.H$/, "_H")
    $CFLAGS << " -DHAVE_#{ending}"

    return res
  end

  if res = have_library(name)
    unless have_header(header) or have_header("SDL/#{header}")
      return false
    end

    return res
  end
end

$CFLAGS << " -pedantic -Wall -std=c99 -Wno-unused-parameter"

unless RUBY_PLATFORM =~ /darwin/
  unless have_library("SDL", "SDL_Init")
    $stdder.puts "SDL not found"
  end

  unless (have_header("SDL/SDL.h") or have_header("SDL.h"))
    $stderr.puts "SDL.h not found."
    exit 1
  end

  unless have_library("SDLmain")
    $stderr.puts "SDL_main not found."
    exit 1
  end

  if have_library("SDL_image")
    have_header("SDL/SDL_image.h") or have_header("SDL_image.h")
  end

  if have_library("SDL_gfx")
    have_header("SDL/SDL_rotozoom.h") or have_header("SDL_rotozoom.h")
  end

  if have_library("SDL_ttf")
    have_header("SDL/SDL_ttf.h") or have_header("SDL_ttf.h")
  end

  create_makefile("ray_ext")

  data = File.read("Makefile").gsub("ray_osx.o", "")
  open("Makefile", 'w') { |f| f.write(data) }
else
  $CFLAGS  << " #{ENV["CFLAGS"]}"
  $LDFLAGS << " #{ENV["LDFLAGS"]}"

  $CFLAGS << " -DRAY_USE_FRAMEWORK"

  unless have_framework("SDL") and have_framework("Cocoa") and
      have_framework("Foundation")
    $stderr.puts "missing frameworks"
    exit 1
  end

  have_sdl_ext("SDL_image", "SDL_image.h")
  have_sdl_ext("SDL_gfx", "SDL_rotozoom.h")
  have_sdl_ext("SDL_ttf", "SDL_ttf.h")

  create_makefile("ray_ext")
end
