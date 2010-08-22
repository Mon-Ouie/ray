$: << "./lib" << "./rspec/lib" << "./ruby_stdlib/lib" << "./psp/lib"

def path_of(res)
  File.expand_path(File.join(File.dirname(__FILE__), "spec", "res", res))
end

puts "requiring ray..."
require 'ray'

puts "requiring rspec..."
require 'spec'

puts "loading specs..."
Dir["spec/**/*_spec.rb"].each { |file| load file }

puts "running spec"
Spec::Runner.run
