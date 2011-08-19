module Ray
  class NoKeyError < StandardError
    def initialize(key)
      super "key #{key} doesn't exist"
    end
  end
  # This is the module including all of your matchers as private methods,
  # allowing you to use them when you call on.
  module Matchers
    module_function

    # @return [DSL::Matcher] An anonymous matcher, using your block to
    #   know if the argument matches.
    #
    # @example
    #   on :foo, where { |i| i > 10 } do |i|
    #     puts "#{i} is greater than 10!"
    #   end
    def where(&block)
      DSL::Matcher.new { |o| block.call(o) }
    end

    # @return [Ray::Key] A key object representing the key of that name
    def key(name)
      Key.new(name)
    end

    # @return [Ray::KeyMod] A key_mod object representing the mod combination
    #   of that name
    def key_mod(name)
      KeyMod.new(name)
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

  # Describes a new matcher. It will be defined as a module function in
  # {Ray::DSL::Matchers}.
  #
  # @param [Symbol] name The name you'll use to create your matcher.

  # @yield The block is called when creating the matcher.
  # @yieldparam *args Arguments passed to the matcher.
  # @yieldreturn [Proc] A proc returning taking a single argument and returning
  #   true when its argument should be matched by the matcher.
  #
  # @example
  #   Ray.describe_matcher(:match) do |regex|
  #     lambda { |str| str =~ regex }
  #   end
  def describe_matcher(name, &create_block)
    Matchers.module_eval do
      define_method(name) do |*args|
        DSL::Matcher.new(&create_block.call(*args))
      end

      module_function name
    end
  end

  module_function :describe_matcher

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
    rect = args.size > 1 ? Ray::Rect.new(*args) : args.first.to_rect
    lambda { |o| o.inside? rect }
  end

  # @overload outside(x, y[, w, h])
  # @overload outside(rect)
  # @overload outside(array)
  #
  # @return [DSL::Matcher] A matching matching any rect outside the argument.
  describe_matcher(:outside) do |*args|
    rect = args.size > 1 ? Ray::Rect.new(*args) : args.first.to_rect
    lambda { |o| o.outside? rect }
  end

  # @overload colliding_with(x, y[, w, h])
  # @overload colliding_with(rect)
  # @overload colliding_with(array)
  #
  # @return [DSL::Matcher] A matching matching any rect colliding with the
  #                        argument.
  describe_matcher(:colliding_with) do |*args|
    rect = args.size > 1 ? Ray::Rect.new(*args) : args.first.to_rect
    lambda { |o| o.collide? rect }
  end

  Keys = {}

  Ray::Event.constants.each do |const|
    if const =~ /^Key(.+)$/
      Keys[$1.downcase.to_sym] = [Ray::Event.const_get(const)]
    end
  end

  Ray::Event.constants.each do |const|
    if const =~ /^Key(?:Num|Kp)(\d+)$/
      (Keys[$1.to_sym] ||= []) << Ray::Event.const_get(const)
    end
  end

  Keys[:number] = Ray::Event.constants.grep(/^KeyNum\d$/).map do |c|
    Ray::Event.const_get(c)
  end

  Keys[:number] |= Ray::Event.constants.grep(/^KeyKp\d$/).map do |c|
    Ray::Event.const_get(c)
  end

  Keys[:letter] = Ray::Event.constants.grep(/^Key[A-Z]$/).map do |c|
    Ray::Event.const_get(c)
  end

  Keys[:function] = Ray::Event.constants.grep(/^KeyF\d+$/).map do |c|
    Ray::Event.const_get(c)
  end

  Keys[:mod] = [Ray::Event::KeyRShift, Ray::Event::KeyLShift,
                Ray::Event::KeyRControl, Ray::Event::KeyLControl,
                Ray::Event::KeyRMeta, Ray::Event::KeyLMeta,
                Ray::Event::KeyRSuper, Ray::Event::KeyLSuper]

  Keys[:arrow] = [Ray::Event::KeyUp, Ray::Event::KeyDown,
                  Ray::Event::KeyLeft, Ray::Event::KeyRight]

  Keys[:+] = [Ray::Event::KeyPlus]
  Keys[:-] = [Ray::Event::KeyMinus]

  Mod = {}

  Ray::Event.constants.each do |const|
    if const =~ /^Mod(.+)$/
      Mod[$1.downcase.to_sym] = [Ray::Event.const_get(const)]
    end
  end

  # A Key object can be used as a matcher to check if a key has been pressed or
  # released:
  #
  #    on :key_press, Key.new(:+) do
  #      # work
  #    end
  #
  # @see KeyMod
  class Key
    def initialize(name)
      raise NoKeyError, name unless Keys[name]
      @symbol = name.to_sym
    end

    def to_a
      Keys[@symbol]
    end

    def to_sym
      @symbol
    end

    def ===(other)
      to_a.include? other
    end

    def inspect
      "key(#{to_sym})"
    end
  end

  # A KeyMod object can be used as a matcher to check if some modifiers are held
  # when a key is pressed.
  #
  # @see KeyMod
  class KeyMod
    def initialize(name)
      raise NoKeyError, name unless Mod[name]
      @symbol = name.to_sym
    end

    def to_a
      Mod[@symbol]
    end

    def to_sym
      @symbol
    end

    def ===(other)
      other.is_a?(Integer) && to_a.any? { |const| (o & const) != 0 }
    end

    def inspect
      "key_mod(#{to_sym})"
    end
  end
end
