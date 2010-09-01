$: << "./ext/" << "./lib/"

def path_of(res)
  File.expand_path(File.join(File.dirname(__FILE__), "spec", "res", res))
end

require 'ray'

require 'spec'
require 'spec/autorun'
Dir["spec/**/*_spec.rb"].each { |file| load file }
