# -*- coding: utf-8 -*-

begin
  require 'devkit' # only used on windows
rescue LoadError
end

begin
  require 'yard'

  YARD::Rake::YardocTask.new do |t|
    t.files = ['lib/**/*.rb', 'ext/**/*.c']
    t.options |= ["--private", "-e", "./yard_ext.rb"]
  end
rescue LoadError
  $stderr.puts("YARD is not installed. Please install it " +
               "with the following command: gem install yard")
end

begin
  require 'jeweler'

  Jeweler::Tasks.new do |s|
    s.name = "ray"

    s.version = "0.1.0pre"

    s.summary     = "A library to write games (or to play with) in Ruby"
    s.description = "Written to play with Ruby"
    s.homepage    = "http://github.com/Mon-Ouie/ray"

    s.email   = "mon.ouie@gmail.com"
    s.authors = ["Mon ouïe"]

    s.files |= FileList["lib/**/*.rb"]
    s.files |= FileList["ext/**/*.{c,h}"]
    s.files |= FileList["ext/vendor/**/*"]
    s.files |= FileList["*.md"]
    s.files << ".gemtest" << "Rakefile"

    s.extensions = ["ext/extconf.rb"]

    s.add_development_dependency "jeweler"
    s.add_development_dependency "yard"
    s.add_development_dependency "riot"
    s.add_development_dependency "rr"
  end
rescue LoadError
  $stderr.puts("Jeweler is not installed. Please install it " +
               "with the following command: gem install jeweler")
end

namespace :ext do
  desc "Rebuilds the C extension"
  task :rebuild do
    cd "ext/"
    ruby "extconf.rb" and sh "make clean && make"
  end

  desc "Builds the C extension"
  task :build do
    cd "ext/"
    ruby "extconf.rb" and sh "make"
  end
end

desc "Builds the C extension"
task :ext => ["ext:build"]

desc "Runs tests"
task :test do
  test_file = File.expand_path("test/run_all.rb", File.dirname(__FILE__))

  begin
    if defined?(RUBY_ENGINE) && RUBY_ENGINE == "jruby"
      ruby "-X+O", test_file
    else
      ruby test_file
    end
  rescue
    exit 1
  end
end
