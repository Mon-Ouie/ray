unless defined? Ray
  unless RUBY_PLATFORM =~ /darwin/
    require 'ray_ext'
  else
    $stderr.puts "Please run this program using ray: ray #$0"
    exit 1
  end
end

require 'ray/color'
require 'ray/rect'
require 'ray/dsl'
