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
    ruby test_file
  rescue
    exit 1
  end
end
