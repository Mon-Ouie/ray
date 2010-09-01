$: << File.expand_path(File.join(File.dirname(__FILE__), "..", "ext"))
$: << File.expand_path(File.join(File.dirname(__FILE__), "..", "lib"))

def path_of(res)
  File.expand_path(File.join(File.dirname(__FILE__), "res", res))
end

require 'ray'
