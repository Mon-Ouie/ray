# -*- coding: utf-8 -*-
begin
  require 'yard'

  YARD::Rake::YardocTask.new do |t|
    t.files = ['lib/**/*.rb', 'ext/**/*.c', 'psp/*.c']
    t.options |= ["--private", "-p", "yard_template", "-e", "./yard_ext.rb"]
  end
rescue LoadError
  $stderr.puts("YARD is not installed. Please install it " +
               "with the following command: gem install yard")
end

begin
  require 'jeweler'

  Jeweler::Tasks.new do |s|
    s.name = "ray"

    s.summary = ""
    s.homepage = "http://github.com/Mon-Ouie/ray"

    s.email = "mon.ouie@gmail.com"
    s.authors = ["Mon ouïe"]

    s.files |= FileList["lib/**/*.rb"]
    s.files |= FileList["ext/**/*.{c,h,m}"]

    s.has_rdoc = "yard"

    s.extensions << "ext/extconf.rb"
  end
rescue LoadError
  $stderr.puts("Jeweler is not installed. Please install it " +
               "with the following command: gem intall jeweler")
end

begin
  require 'spec/rake/spectask'

  Spec::Rake::SpecTask.new('spec') do |t|
    t.spec_files = FileList['spec/**/*_spec\.rb']
    t.fail_on_error = false
  end
rescue LoadError
  $stderr.puts("RSpec is not installed. Please install it " +
               "with the following command: gem install rspec")
end

namespace :ext do
  desc "Rebuilds the C extension"
  task :rebuild do
    cd "ext/"
    sh "ruby extconf.rb && make clean && make"
  end

  desc "Builds the C extension"
  task :build do
    cd "ext/"
    sh "ruby extconf.rb && make"
  end
end

desc "Builds the C extension"
task :ext => ["ext:build"]

begin
  require 'rake/psp_task'

  libs = %w(socket/socket.c socket/getnameinfo.c socket/getaddrinfo.c
          socket/pspsocket.c
          bigdecimal/bigdecimal.c
          digest/sha2/sha2init.c digest/sha2/sha2.c
          digest/rmd160/rmd160.c digest/rmd160/rmd160init.c
          digest/bubblebabble/bubblebabble.c
          digest/sha1/sha1.c digest/sha1/sha1init.c
          digest/md5/md5.c digest/md5/md5init.c
          digest/digest.c
          enumerator/enumerator.c
          fcntl/fcntl.c
          stringio/stringio.c
          strscan/strscan.c
          thread/thread.c
          zlib/zlib.c
          nkf/nkf.c
          etc/etc.c
          syck/bytecode.c syck/gram.c syck/implicit.c syck/rubyext.c
          syck/token.c syck/emitter.c syck/handler.c syck/node.c syck/syck.c
          syck/yaml2byte.c).map { |i| "psp/#{i}" }

  static_libs = ["-lSDL_gfx", "-lSDL_image", "-lSDL", "-lGL", "-lpspvfpu",
                 "-lpspgu", "-lpspaudio", "-lpsprtc", "-lpsphprm", "-lpspwlan",
                 "-lruby", "-lpng", "-ljpeg", "-lz","-lm", "-lc"]

  flags = ["-Wall", "-Wno-unused-parameter", "-D_PSP_ -DHAVE_STRUCT_TIMESPEC",
           "-DHAVE_SDL_SDL_IMAGE_H", "-G0", "-DHAVE_SDL_SDL_ROTOZOOM_H"]

  begin
    PSPTask.new do |t|
      t.objdir  = "./obj/"
      t.srcs    = Dir['ext/*.c', 'psp/*.c'] | libs
      t.libs    = static_libs | t.libs
      t.cflags |= flags
      t.incdir << "-I./ext" << "-I./psp"
      t.target  = "ray_release"

      t.build_prx   = true
      t.build_eboot = true
    end

    PSPTask.new(:debug) do |t|
      t.objdir  = "./obj_dbg/"
      t.srcs    = Dir['ext/*.c', 'psp/*.c'] | libs
      t.libs    = static_libs | t.libs
      t.cflags |= (flags | ["-O0", "-g"])
      t.incdir << "-I./ext" << "-I./psp"
      t.target  = "ray"

      t.build_prx = true
    end
  rescue
    $stderr.puts "could not create PSP tasks"
  end
rescue LoadError
  $stderr.puts("psp_task is not installed. Please install it " +
               "with the following command: gem intall psp_task")
end

require 'net/ftp'

file "ruby-1.8.7-p248.tar.bz2" do
  ftp = Net::FTP.new
  ftp.connect("ftp.ruby-lang.org", 21)
  ftp.login("anonymous", "")
  puts "Downloading ruby-1.8.7-p248.tar.bz2..."
  ftp.getbinaryfile("pub/ruby/1.8/ruby-1.8.7-p248.tar.bz2",
                    "ruby-1.8.7-p248.tar.bz2")
  ftp.close
end

directory "ruby_stdlib"
file "ruby_stdlib" => ["ruby-1.8.7-p248.tar.bz2"] do
  sh "tar xvf ruby-1.8.7-p248.tar.bz2"
  cp_r "ruby-1.8.7-p248/lib", "ruby_stdlib"

  cp_r "psp/bigdecimal/lib/bigdecimal", "ruby_stdlib/lib"

  mkdir_p "ruby_stdlib/lib/digest" unless File.exist? "ruby_stdlib/lib/digest"
  cp "psp/digest/lib/digest.rb", "ruby_stdlib/lib"
  cp "psp/digest/lib/sha1.rb", "ruby_stdlib/lib/digest"
  cp "psp/digest/lib/md5.rb", "ruby_stdlib/lib/digest"

  cp "psp/nkf/lib/kconv.rb", "ruby_stdlib/lib"
end

ruby_libs = ["socket.rb", "bigdecimal.rb", "digest/bubblebabble.rb",
             "fcntl.rb", "stringio.rb", "strscan.rb", "thread.rb",
             "zlib.rb", "nkf.rb", "etc.rb", "syck.rb", "enumerator.rb"]

ruby_libs.each do |filename|
  ext_name = case filename
             when /(\w+)\.rb/
               $1
             when /\w+\/(\w+)\.rb/
               $1
             else
               filename
             end

  dir = File.dirname("ruby_stdlib/lib/#{filename}")

  directory dir
  file "ruby_stdlib/lib/#{filename}" => ["ruby_stdlib", dir] do
    open("ruby_stdlib/lib/#{filename}", 'w') do |file|
      file.puts "Ray.init_internal(\"#{ext_name}\")"
    end
  end
end

ruby_libs.map! { |i| "ruby_stdlib/lib/#{i}" }

desc "Builds dependencies for running the specs on a PSP"
task :psp_spec => (['debug:prx'] | ruby_libs)
