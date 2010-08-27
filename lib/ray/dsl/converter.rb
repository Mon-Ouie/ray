module Ray
  module DSL
    # Module storing all the regitred converters. You should never
    # need to use it directly.
    module Converter
      @@converters = {}

      class << self
        def add_converter(from, to, &block)
          @@converters[[Ray.resolve_type(from), Ray.resolve_type(to)]] = block
        end

        def convert(obj, target)
          unless target.is_a? Module
            return convert(obj, Ray.resolve_type(target))
          end

          if converter = @@converters[[obj.class, target]] # Direct converter
            return converter.call(obj)
          end

          @@converters.each do |(from, to), converter|
            if obj.is_a?(from) && to.ancestors.include?(target)
              return converter.call(obj)
            end
          end

          if target == Integer
            return obj.to_i if obj.respond_to? :to_i
          elsif target == Float
            return obj.to_f if obj.respond_to? :to_f
          elsif target == String
            return obj.to_s if obj.respond_to? :to_s
          end

          raise TypeError, "Can't convert #{obj.class} into #{target}"
        end
      end
    end
  end

  # Converts an object to a given type. target can be a Module or
  # an object registred as a type.
  #
  # If there is a converter that can directly convert from obj.class to target,
  # it will be used. If there isn't, it will see it will search in
  # obj.class.ancestors and in target's subclasses.
  #
  # @param [Object] obj The object which should be converted
  # @param [Module, Symbol] target The targeted type
  def self.convert(obj, target)
    DSL::Converter.convert(obj, target)
  end

  # Adds a block telling to ray how to convert from a type to another one.
  #
  # @example Converting from String to Array
  #   Ray.describe_conversion(:string => Array) do |s| # registred type or module
  #     s.split('')
  #   end
  # @example declaring multiple conversions at once
  #   Ray.describe_conversion(:string => Array, :string => Object) do |s|
  #     s.split('')
  #   end
  def self.describe_conversion(hash, &block)
    hash.each do |from, to|
      DSL::Converter.add_converter(from, to, &block)
    end
  end
end
