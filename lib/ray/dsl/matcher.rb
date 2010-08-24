module Ray
  # This is the module including all of your matchers as private methods,
  # allowing you to use them when you call on.
  module Matchers; end

  module DSL
    class Matcher
      def initialize(target, &block)
        @target = Ray.resolve_type(target)
        @block = block
      end

      # @return [true, false] True if we can match on the object of that class
      def can_match_on?(klass)
        Ray.resolve_type(klass).ancestors.include? @target
      end

      # @return [true, false] True if the block this object was created with
      #                       returns true when called with obj.
      def match?(obj)
        can_match_on?(obj.class) && @block.call(obj)
      end

      alias :=== :match?
    end
  end

  # Describes a new matcher.
  #
  # @param [Symbol] name The name you'll use to call your matcher
  # @param [Symbol, Module] target the type on which the matcher operates.
  # @param [Proc] create_block a block called with the arguments of your matcher
  #                            method, and returning the block that will be used
  #                            to check if the condition is matched.
  # @example
  #   Ray.describe_matcher(:match, :string) do |regex|
  #     lambda { |str| str =~ regex }
  #   end
  def self.describe_matcher(name, target, &create_block)
    Matchers.module_eval do
      define_method(name) do |*args|
        DSL::Matcher.new(target, &create_block.call(*args))
      end

      private name
    end
  end
end
