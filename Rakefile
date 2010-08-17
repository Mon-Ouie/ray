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

desc "Build C extension"
task :ext do
  cd "ext/"
  sh "ruby extconf.rb && make clean && make"
end
