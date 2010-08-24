require 'singleton'

module Ray
  # A class holding program wide settings.
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

    # Clears all the settings
    def clear
      @settings.clear
    end

    # Sets the value for a key.
    # If value is a Hash, and if it contains only one element, and if the
    # only key of this hash is a known type, it will convert the value to
    # the one represented by the key.
    #
    # @example A conversion
    #   set :foo, :string => 3 # set :foo, "3"
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

    # Removes the value for this key.
    def unset(key)
      @settings.delete(key)
    end
  end
end
