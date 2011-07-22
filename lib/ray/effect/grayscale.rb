module Ray
  class Effect
    # A grayscale effect. To accomplish it, it computes the dot product of the
    # color by a ratio, and assigns the result to red, green, and blue
    # components. Alpha component is preserved.
    class Grayscale < Effect
      effect_name :grayscale
      attribute   :ratio, :vec3

      # @param [Ray::Vector3] ratio Default ratio
      def initialize(ratio = [0.299, 0.587, 0.114])
        @ratio = ratio
      end

      # @return [Ray::Vector3] ratio
      attr_accessor :ratio

      def defaults
        {:ratio => @ratio}
      end

      def code
        return <<code
vec4 do_grayscale(ray_grayscale args, vec4 color) {
  float gray = dot(color.rgb, args.ratio);
  return vec4(gray, gray, gray, color.a);
}
code
      end
    end
  end
end
