module Ray
  class Color
    class << self
      def red;     new(255, 0, 0);     end
      def green;   new(0, 255, 0);     end
      def blue;    new(0, 0, 255);     end
      def black;   new(0, 0,0);        end
      def white;   new(255, 255, 255); end
      def none;    new(0, 0, 0, 0);    end
      def gray;    new(128, 128, 128); end
      def cyan;    new(0, 255, 255);   end
      def yellow;  new(255, 255, 0);   end
      def fuschia; new(255, 0, 255);   end

      # @param [Float] hue Hue, between 0 and 360
      # @param [Float] sat Saturation, between 0 and 1
      # @param [Float] val Value, between 0 and 1
      #
      # @return [Ray::Color] Color created from those parameters
      def from_hsv(hue, sat, val)
        hue, sat, val = hue.to_f, sat.to_f, val.to_f

        if sat == 0
          new(val * 255, val * 255, val * 255)
        else
          i = (hue / 60).floor % 6
          f = (hue / 60) - i
          l = val * (1 - sat)
          m = val * (1 - f * sat)
          n = val * (1 - (1 - f) * sat)

          case i
          when 0 then new(val * 255, n * 255, l * 255)
          when 1 then new(m * 255, val * 255, l * 255)
          when 2 then new(l * 255, val * 255, n * 255)
          when 3 then new(l * 255, m * 255, val * 255)
          when 4 then new(n * 255, l * 255, val * 255)
          when 5 then new(val * 255, l * 255, m * 255)
          end
        end
      end
    end

    def to_color
      self
    end

    def to_s
      "RGBA(#{r}, #{g}, #{b}, #{a})"
    end

    # @param [Ray::Color] color Another color.
    # @return [Ray::Color] Sum of two colors, adding component pairs.
    def +(color)
      r = red   + color.red
      g = green + color.green
      b = blue  + color.blue
      a = alpha + color.alpha

      Ray::Color.new(r > 255 ? 255 : r,
                     g > 255 ? 255 : g,
                     b > 255 ? 255 : b,
                     a > 255 ? 255 : a)
    end

    # @param [Ray::Color] color Another color.
    # @return [Ray::Color] Product of two colors, multiplying component pairs
    #   before dividing everything by 255.
    def *(color)
      Ray::Color.new(r * color.r / 255.0,
                     g * color.g / 255.0,
                     b * color.b / 255.0,
                     a * color.a / 255.0)
    end

    def ==(obj)
      return false unless obj.is_a? Color
      r == obj.r && g == obj.g && b == obj.b && a == obj.a
    end

    def eql?(obj)
      self.class == obj.class && self == obj
    end

    def hash
      [r, g, b, a].hash
    end

    def to_a
      [r, g, b, a]
    end

    alias :red :r
    alias :green :g
    alias :blue :b
    alias :alpha :a

    alias :red= :r=
    alias :green= :g=
    alias :blue= :b=
    alias :alpha= :a=
  end
end
