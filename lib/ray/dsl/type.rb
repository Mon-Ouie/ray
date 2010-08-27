module Ray
  module DSL
    # The module that knows about all the type you registred.
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

  # Registers a type. It allows you to say name instead of klass
  # when Ray asks you for a typename.
  #
  # @param [Symbol] name The object that will represent klass
  # @param [Module] klass The class name will be resolved as
  def self.register_type(name, klass)
    DSL::Type.register_type(name, klass)
  end

  # @return [true, false] True if name is a known type
  def self.know_type?(name)
    DSL::Type.know_type?(name)
  end

  # @param [Module, Symbol] name An object supposed to identify a module or a
  #                              class.
  # @return [Module, nil] The matchig module
  def self.resolve_type(name)
    if name.is_a? Module
      name
    else
      DSL::Type.resolve_type(name)
    end
  end

  register_type(:image, Ray::Image)
  register_type(:color, Ray::Color)

  register_type(:string, String)
  register_type(:array, Array)
  register_type(:hash, Hash)
  register_type(:integer, Integer)
  register_type(:float, Float)
  register_type(:numeric, Numeric)

  register_type(:anything, Object)
end
