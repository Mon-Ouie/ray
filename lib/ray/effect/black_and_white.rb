module Ray
  class Effect
    # A black and white effect. It considers the grayscale level of the image,
    # and sets pixels that are darker than a given value to black, and the
    # others to white.
    class BlackAndWhite < Effect
      effect_name :black_and_white
      attribute   :ratio, :vec3
      attribute   :value, :float

      # @param [Ray::Vector3] ratio Default ratio to compute grayscale
      # @param [Float] value Minimal grayscale level of white pixels
      def initialize(value = 0.5, ratio = [0.299, 0.587, 0.114])
        @ratio = ratio
        @value = value
      end

      # @return [Ray::Vector3] ratio
      attr_accessor :ratio

      def defaults
        {:ratio => @ratio, :value => @value}
      end

      def code
        return <<code
vec4 do_black_and_white(ray_black_and_white args, vec4 color) {
  float gray = dot(color.rgb, args.ratio);
  if (gray > args.value)
    return vec4(1, 1, 1, color.a);
  else
    return vec4(0, 0, 0, color.a);
}
code
      end
    end
  end
end
