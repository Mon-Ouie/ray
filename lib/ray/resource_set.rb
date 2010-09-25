module Ray
  # Exception raised when a pattern isn't found in ResourceSet
  class NoPatternError < StandardError; end

  # Mixin used to cache resources. It allows to register different block
  # to create the resources using a regexp:
  #   add_set(/^http://(.+)$/) do |url| # captures are passed to the block
  #     ...
  #   end
  #
  # Custom arguments can also be specified:
  #   add_set(/^http://(.+)/) do |url, priority| # arguments are passed at the end
  #     ...
  #   end
  #
  # @see Ray::ResourceSet::ClassMethods
  #
  module ResourceSet
    module ClassMethods
      # Registers a block for handling some resources.
      def add_set(regexp, &block)
        set_hash[regexp] = Hash.new do |hash, args|
          regexp =~ args.first

          block_args = $~.captures + args[1..-1]
          hash[args] = block.call(*block_args)
        end
      end

      def set_hash
        (@set_hash ||= {})
      end

      # @return The object corresponding to a given list of parameters.
      def [](value, *params)
        if params.size != required_argument_count
          raise(ArgumentError, "wrong number of arguments (%d for %d)" %
                [params.size, required_argument_count])
        end

        set_hash.each do |regexp, hash|
          return hash[([value] + params)] if regexp =~ value
        end

        missing_pattern(value)
      end

      # Selects objects matching a condition.
      #
      # @yield *arguments, object
      # @yieldparam *arguments All the arguments used to access that object
      # @yieldparam object the actual resource
      def select!(&block)
        return Enumerator.new(self, :select!) unless block_given?

        set_hash.each do |regexp, hash|
          hash.delete_if do |key, val|
            !block.call(*(key + [val]))
          end
        end
      end

      # Removes all the objects from the array but those for which
      # block returned true.
      #
      # @yield *arguments, object (See #select!)
      def reject!(&block)
        return Enumerator.new(self, :reject!) unless block_given?
        select! { |*args| !block.call(*args) }
      end

      alias :delete_if :reject!

      # Sets the required argument count. If it is set to one, then
      # ["a", "b"] is valid but not ["a"], since the first argument is always
      # required.
      #
      # Defaulted to 0.
      def need_argument_count(n)
        @set_argument_count = n
      end

      # Returns the required argument count.
      def required_argument_count
        @set_argument_count ||= 0
      end

      # Method called when a string isn't matched by any pattern.
      # Raises a NoPatternError excpetion by default.
      def missing_pattern(string)
        raise NoPatternError, "#{string.inspect} not macthed by any pattern"
      end
    end

    def self.included(by)
      by.extend(ClassMethods)
    end
  end
end
