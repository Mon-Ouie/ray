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
  # Notice you would usually use extend instead of include to use this module.
  #
  module ResourceSet
    # Registers a block for handling some resources.
    def add_set(regexp, &block)
      set_hash[regexp] = Hash.new do |hash, key|
        key =~ regexp
        hash[key] = yield(*$~.captures)
      end
    end

    def set_hash
      (@set_hash ||= {})
    end

    # @return The object corresponding to a given string.
    def [](key)
      set_hash.each do |regexp, hash|
        return hash[key] if regexp =~ key
      end

      missing_pattern(key)
    end

    # Selects objects matching a condition.
    #
    # @yield key, object
    # @yieldparam key the key used to refer to the resource
    # @yieldparam object the actual resource
    def select!
      return Enumerator.new(self, :select!) unless block_given?

      set_hash.each do |regexp, hash|
        hash.delete_if { |key, val| !yield(key, val) }
      end
    end

    # Removes all the objects from the array but those for which
    # block returned true.
    #
    # @yield key, object (See #select!)
    def reject!
      return Enumerator.new(self, :reject!) unless block_given?
      select! { |key, obj| !yield(key, obj) }
    end

    alias :delete_if :reject!

    # Removes all the items from the resource set
    def clear
      set_hash.each { |regexp, hash| hash.clear }
    end

    # Method called when a string isn't matched by any pattern.
    # Raises a NoPatternError excpetion by default.
    def missing_pattern(string)
      raise NoPatternError, "#{string.inspect} not macthed by any pattern"
    end
  end
end
