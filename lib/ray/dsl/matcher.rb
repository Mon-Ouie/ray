module Ray
  # This is the module including all of your matchers as private methods,
  # allowing you to use them when you call on.
  module Matchers
    # @return [DSL::Matcher] An anonymous matcher, using your block to
    #                        know if the argument matches.
    #
    # @example
    #   on :foo, where { |i| i > 10 } do |i|
    #     puts "#{i} is greater than 10!"
    #   end
    def where(&block)
      DSL::Matcher.new { |o| block.call(o) }
    end
  end

  module DSL
    class Matcher
      def initialize(&block)
        @block = block
      end

      # @return [true, false] True if the block this object was created with
      #                       returns true when called with obj.
      def match?(obj)
        @block.call(obj)
      end

      alias :=== :match?
    end
  end

  # Describes a new matcher.
  #
  # @param [Symbol] name The name you'll use to call your matcher
  # @param [Proc] create_block a block called with the arguments of your matcher
  #                            method, and returning the block that will be used
  #                            to check if the condition is matched.
  # @example
  #   Ray.describe_matcher(:match, :string) do |regex|
  #     lambda { |str| str =~ regex }
  #   end
  def self.describe_matcher(name, &create_block)
    Matchers.module_eval do
      define_method(name) do |*args|
        DSL::Matcher.new(&create_block.call(*args))
      end
    end
  end

  # @return [DSL::Matcher] A matcher matching anything (always true)
  describe_matcher(:anything) do
    lambda { |o| true }
  end

  # @return [DSL::Matcher] A matcher matching anything greater than x
  #                        (comparaison using >)
  describe_matcher(:more_than) do |x|
    lambda { |o| o > x }
  end

  # @return [DSL::Matcher] A matcher matching anything that is less than x
  #                        (comparaison using <)
  describe_matcher(:less_than) do |x|
    lambda { |o| o < x }
  end

  # @return [DSL::Matcher] A matcher matching a value close of x.
  # @note the maximum and the minimum will only be computed once.
  #
  # @example
  #   on :win, almost(10_000, 500) do ... end
  #   on :win, where { |x| x <= 10_000 + 500 && x >= 10_000 - 500 } do ... end
  describe_matcher(:almost) do |x, precision|
    min, max = (x - precision), (x + precision)
    lambda { |o| (o <= max) && (o >= min) }
  end

  # @overload inside(x, y[, w, h])
  # @overload inside(rect)
  # @overload inside(array)
  #
  # @return [DSL::Matcher] A matching matching any rect inside the argument.
  describe_matcher(:inside) do |*args|
    rect = args.size > 1 ? Ray::Rect.new(*args) : Ray.convert(args.first, :rect)
    lambda { |o| o.inside? rect }
  end

  # @overload outside(x, y[, w, h])
  # @overload outside(rect)
  # @overload outside(array)
  #
  # @return [DSL::Matcher] A matching matching any rect outside the argument.
  describe_matcher(:outside) do |*args|
    rect = args.size > 1 ? Ray::Rect.new(*args) : Ray.convert(args.first, :rect)
    lambda { |o| o.outside? rect }
  end

  # @overload colliding_with(x, y[, w, h])
  # @overload colliding_with(rect)
  # @overload colliding_with(array)
  #
  # @return [DSL::Matcher] A matching matching any rect colliding with the
  #                        argument.
  describe_matcher(:colliding_with) do |*args|
    rect = args.size > 1 ? Ray::Rect.new(*args) : Ray.convert(args.first, :rect)
    lambda { |o| o.collide? rect }
  end

  KEYS = Ray::Event.constants.inject({}) do |hash, const|
    if const =~ /^KEY_(.+)$/
      hash[$1.downcase.to_sym] = [Ray::Event.const_get(const)]
    elsif const =~ /^PSP_BUTTON_(.+)$/
      hash["psp_#{$1.downcase.to_sym}".to_sym] = [Ray::Event.const_get(const)]
    end

    hash
  end

  KEYS[:number] = Ray::Event.constants.select { |c| c =~ /^KEY_\d$/ }.map do |c|
    Ray::Event.const_get(c)
  end

  KEYS[:number] |= Ray::Event.constants.select { |c| c =~ /^KEY_KP\d$/ }.map do |c|
    Ray::Event.const_get(c)
  end

  KEYS[:letter] = Ray::Event.constants.select { |c| c =~ /^KEY_[a-z]$/ }.map do |c|
    Ray::Event.const_get(c)
  end

  KEYS[:function] = Ray::Event.constants.select { |c| c =~ /^KEY_F\d$/ }.map do |c|
    Ray::Event.const_get(c)
  end

  KEYS[:mod] = [Ray::Event::KEY_RSHIFT, Ray::Event::KEY_LSHIFT,
                Ray::Event::KEY_RCTRL, Ray::Event::KEY_LCTRL,
                Ray::Event::KEY_RALT, Ray::Event::KEY_LALT,
                Ray::Event::KEY_RMETA, Ray::Event::KEY_LMETA,
                Ray::Event::KEY_RSUPER, Ray::Event::KEY_LSUPER]

  KEYS[:arrow] = [Ray::Event::KEY_UP, Ray::Event::KEY_DOWN,
                  Ray::Event::KEY_LEFT, Ray::Event::KEY_RIGHT]

  MOD = Ray::Event.constants.inject({}) do |hash, const|
    if const =~ /^KMOD_(.+)$/
      hash[$1.downcase.to_sym] = [Ray::Event.const_get(const)]
    end

    hash
  end

  # @return [DSL::Matcher] A matcher matching the given key, which is a symbol
  #                        like :space, :a, :b, :number, :letter, :arrow, ...
  describe_matcher(:key) do |sym|
    ary = KEYS[sym.to_sym]
    lambda { |o| ary.include? o }
  end

  # @return [DSL::Matcher] A matcher matching the given modifier key
  #                        (:rctrl, :lctrl, :rmeta, :lmeta, ...)
  describe_matcher(:key_mod) do |sym|
    ary = MOD[sym.to_sym]
    lambda { |o| ary.detect { |const| o & const } }
  end
end
