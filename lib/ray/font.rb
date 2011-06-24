module Ray
  class Font
    include Ray::TextHelper

    extend Ray::ResourceSet
    add_set(/^(.*)$/) { |filename| new(filename) }
  end
end
