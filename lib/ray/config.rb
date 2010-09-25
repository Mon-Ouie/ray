require 'singleton'

module Ray
  # A class holding program wide settings.
  #
  # @example
  #   Ray::Config.config do
  #     set :value, :string => 3 # Conversion
  #     set :value, "3"
  #     set :value, {3 => 4} # Ray::Config[:value] == {3 => 4}
  #     set :value, :string => 3, :hash => {} # No conversion either
  #     unest :value # set to nil
  #   end
  class Config
    include Singleton
    include Enumerable

    class << self
      # @return [Object] the value for key
      def [](key)
        instance[key]
      end

      # Yields the instance of Config if the block takes an argument.
      # Uses instance_eval in other cases.
      def config(&block)
        if block.arity == 1
          block.call(instance)
        else
          instance.instance_eval(&block)
        end
      end

      # Clears all the settings, and then calls config.
      def config!(&block)
        instance.clear
        config(&block)
      end
    end

    def initialize
      @settings = {}
    end

    # @return [Object] The value for key
    def [](key)
      @settings[key]
    end

    # Sets the value for key
    def []=(key, value)
      @settings[key] = value
    end

    def each(&block)
      @settings.each(&block)
    end

    # Clears all the settings
    def clear
      @settings.clear
    end

    # @overload set(key, value)
    #   Sets a value for a key.
    #
    # @overload set(key, hash)
    #   Sets a value for a key.
    #   @param [Hash] hash If hash.size == 1, and if its only key is a type,
    #                      its value will be converted to that type and assigned
    #                      to key. Otherwise, it will set key to hash.
    #
    #   @example A conversion
    #     set :foo, :string => 3 # set :foo, "3"
    #   @example No conversion
    #     set :foo, :string => 3, :array => 3
    #     set :foo, "foobar" => 3
    def set(key, value)
      if value.is_a?(Hash) && value.size == 1
        type, val = value.keys.first, value.values.first

        if Ray.know_type? type
          self[key] = Ray.convert(val, type)
        else
          self[key] = value
        end
      else
        self[key] = value
      end
    end

    # Removes the value for a key.
    def unset(key)
      @settings.delete(key)
    end
  end
end
