module Ray
  module Matchers; end

  module DSL
    class Matcher
      def initialize(target, &block)
        @target = Ray.resolve_type(target)
        @block = block
      end

      def can_match_on?(klass)
        Ray.resolve_type(klass).ancestors.include? @target
      end

      def match?(obj)
        can_match_on?(obj.class) && @block.call(obj)
      end
    end
  end

  def self.describe_matcher(name, target, &create_block)
    Matchers.module_eval do
      define_method(name) do |*args|
        DSL::Matcher.new(target, &create_block.call(*args))
      end

      private name
    end
  end
end
