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
