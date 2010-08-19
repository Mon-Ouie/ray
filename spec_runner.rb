require 'profile'

$: << "./ext/" << "./lib/"
path = File.expand_path(File.join(File.dirname(__FILE__), "ext/ray"))
our_path = File.expand_path(File.join(File.dirname(__FILE__), "spec_runner.rb"))

unless defined? Ray
  unless RUBY_PLATFORM =~ /darwin/
    require 'ray_ext'
  else
    if File.exist? path
      system "#{path} #{our_path}"
      exit $?.exitstatus
    else
      $stderr.puts "please build ray (rake ext)"
      exit 1
    end
  end
end

require 'ray'

require 'spec'
require 'spec/autorun'
Dir["spec/**/*_spec.rb"].each { |file| load file }
