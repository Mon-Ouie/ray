module Ray
  class AudioSource
    include Ray::PP

    def pretty_print(q, other_attr = [])
      attr = ["pos", "relative?", "min_distance", "attenuation",
              "pitch", "status", "volume"]
      pretty_print_attributes q, attr + other_attr
    end
  end
end
