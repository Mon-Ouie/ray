module Ray
  module DSL
    module Type
      @@types = {}

      class << self
        def register_type(name, klass)
          @@types[name] = klass
        end

        def know_type?(name)
          (@@types[name] != nil)
        end

        def resolve_type(name)
          @@types[name]
        end
      end
    end
  end

  def self.register_type(name, klass)
    DSL::Type.register_type(name, klass)
  end

  def self.know_type?(name)
    DSL::Type.know_type?(name)
  end

  def self.resolve_type(name)
    DSL::Type.resolve_type(name)
  end

  register_type(:image, Ray::Image)
  register_type(:color, Ray::Color)

  register_type(:string, String)
  register_type(:array, Array)
  register_type(:hash, Hash)
  register_type(:integer, Integer)
  register_type(:float, Float)
  register_type(:numeric, Numeric)
end
