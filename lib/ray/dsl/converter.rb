module Ray
  module DSL
    class Converter
      @@converters = {}

      def self.add_converter(from, to, &block)
        @@converters[from => to] = new(&block)
      end

      def self.convert(obj, target)
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

      def initialize(&block)
        @block = block
      end

      def call(obj)
        @block.call(obj)
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

  describe_conversion(String => Integer) { |o| o.to_i }
  describe_conversion(String => Float)   { |o| o.to_f }
  describe_conversion(String => Numeric) { |o| o.to_f }
  describe_conversion(Numeric => String) { |o| o.to_s }
end
