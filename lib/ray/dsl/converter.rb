module Ray
  module DSL
    module Converter
      @@converters = {}

      class << self
        def add_converter(from, to, &block)
          @@converters[Ray.resolve_type(from) => Ray.resolve_type(to)] = block
        end

        def convert(obj, target)
          unless target.is_a? Module
            return convert(obj, Ray.resolve_type(target))
          end

          if converter = @@converters[obj.class => target] # Direct converter
            return converter.call(obj)
          end

          @@converters.each do |hash, converter|
            from, to = hash.keys.first, hash.values.first

            if obj.is_a?(from) && to.ancestors.include?(target)
              return converter.call(obj)
            end
          end

          raise TypeError, "Can't convert #{obj.class} into #{target}"
        end
      end
    end
  end

  def self.convert(obj, target)
    DSL::Converter.convert(obj, target)
  end

  def self.describe_conversion(hash, &block)
    hash.each do |from, to|
      DSL::Converter.add_converter(from, to, &block)
    end
  end

  describe_conversion(:string  => :integer) { |o| o.to_i }
  describe_conversion(:string  => :float)   { |o| o.to_f }
  describe_conversion(:string  => :numeric) { |o| o.to_f }
  describe_conversion(:numeric => :string)  { |o| o.to_s }
end
