$: << "./lib" << "./rspec/lib" << "./ruby_stdlib/lib" << "./psp/lib"

def path_of(res)
  File.expand_path(File.join(File.dirname(__FILE__), "spec", "res", res))
end

require 'ray'
require 'spec'
Dir["spec/**/*_spec.rb"].each { |file| load file }
Spec::Runner.run
