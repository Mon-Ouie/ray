module Ray
  class Joystick
    @@joysticks = Hash.new { |h, k| h[k] = new(k) }

    class << self
      # @return [Ray::Joystick] a joystick, opened if necesarry.
      def [](id)
        joy = @@joysticks[id]
        joy.open if joy.closed?
        joy
      end

      # Enumerates through all the joysitcks.
      #
      # @yield [joystick]
      # @yieldparam [Ray::Joystick] joystick
      def each
        return Enumerator.new(self, :each) unless block_given?

        (0...count).each do |i|
          yield self[i]
        end

        self
      end

      include Enumerable
    end
  end
end
