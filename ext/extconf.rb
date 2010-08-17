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
    $CFLAGS  << " -DHAVE_FRAMEWORK_#{name.upcase}"
  else
    puts "no"
  end
  ret
end

has_run_node = have_func("ruby_run_node")
has_run      = have_func("ruby_run")

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

  create_makefile("ray_ext")

  data = File.read("Makefile").gsub("SDLMain.o", "")
  open("Makefile", 'w') { |f| f.write(data) }
else
  if !has_run and !has_run_node
    $stderr.puts "Unsupported configuration: Mac OS X and a ruby " +
                 "implementation that does not implement ruby_run"
    exit 1
  end

  $CFLAGS << " -DRAY_USE_FRAMEWORK"

  unless have_framework("SDL") and have_framework("Cocoa") and
      have_framework("Foundation")
    $stderr.puts "missing frameworks"
    exit 1
  end

  have_library("ruby", "ruby_init")

  create_makefile("ray_ext")
end

if has_run or has_run_node
  open("Makefile", 'a') do |file|
    file.puts "ray: $(OBJS)"
    file.puts "\t$(CC) -o ray $(OBJS) $(LIBPATH) $(DLDFLAGS) " +
      "$(LOCAL_LIBS) $(LIBS) -lruby"
  end

  data = File.read("Makefile").gsub("all:", "all: ray")
  open("Makefile", 'w') { |f| f.write(data) }
end
