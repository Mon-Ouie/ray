module Ray
  class SoundBuffer
    extend Ray::ResourceSet
    add_set(/^(.*)$/) { |filename| new(filename) }
  end
end
