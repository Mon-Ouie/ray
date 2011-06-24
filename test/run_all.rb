require File.expand_path(File.dirname(__FILE__)) + '/helpers.rb'

Dir["#{File.dirname(__FILE__)}/**/*_test.rb"].each do |filename|
  load filename
end

run_tests
