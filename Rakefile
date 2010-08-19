# -*- coding: utf-8 -*-
begin
  require 'yard'

  YARD::Rake::YardocTask.new do |t|
    t.files = ['lib/**/*.rb', 'ext/**/*.c']
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
    s.files |= FileList["bin/ray{,_irb}"]

    s.extensions << "ext/extconf.rb"

    s.executables << "ray" << "ray_irb"
  end
rescue LoadError
  $stderr.puts("Jeweler is not installed. Please install it " +
               "with the following command: gem intall jeweler")
end

desc "Run specs"
task :spec do
  load "spec_runner.rb"
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

  PSPTask.new do |t|
    t.objdir  = "./obj/"
    t.srcs    = Dir['ext/*.c', 'psp/*.c']
    t.libs    = ["-LSDLmain", "-lSDL", "-lGL", "-lpspvfpu", "-lpspgu",
                 "-lpspaudio", "-lpsprtc", "-lpsphprm", "-lruby",
                 "-lm"] | t.libs
    t.cflags |= ["-Wall", "-std=c99", "-Wno-unused-parameter",
                 "-D_PSP_ -DHAVE_STRUCT_TIMESPEC"]
    t.target  = "ray_release"

    t.build_prx   = true
    t.build_eboot = true
  end

  PSPTask.new(:debug) do |t|
    t.objdir  = "./obj_dbg/"
    t.srcs    = Dir['ext/*.c', 'psp/*.c']
    t.libs    = ["-LSDLmain", "-lSDL", "-lGL", "-lpspvfpu", "-lpspgu",
                 "-lpspaudio", "-lpsprtc", "-lpsphprm", "-lruby",
                 "-lm"] | t.libs
    t.cflags |= ["-Wall", "-std=c99", "-Wno-unused-parameter",
                 "-D_PSP_ -DHAVE_STRUCT_TIMESPEC", "-g", "-O0"]
    t.target  = "ray"

    t.build_prx = true
  end
rescue LoadError
  $stderr.puts("psp_task is not installed. Please install it " +
               "with the following command: gem intall psp_task")
end
