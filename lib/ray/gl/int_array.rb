module Ray
  module GL
    class IntArray
      include Enumerable

      def each
        (0...size).each { |i| yield self[i] }
      end

      # @yield A block changing each value of the array
      # @yieldparam [Integer] val Old value
      # @yieldreturn [Integer] New value
      def map!
        (0...size).each { |i| self[i] = yield self[i] }
      end

      # @yield Each index of the array
      # @yieldparam [Integer] i Index of an element
      def each_index(&block)
        (0...size).each(&block)
      end
    end
  end
end
