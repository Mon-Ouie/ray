module Ray
  class Joystick
    @@joysticks = Hash.new { |h, k| new(k) }
    class << self
      def [](id)
        joy = @@joysticks[id]
        joy.open if joy.closed
        joy
      end

      def each
        (0...count).each do |i|
          yield self[i]
        end
      end

      include Enumerable
    end
  end
end
