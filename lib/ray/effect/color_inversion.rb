module Ray
  class Effect
    # An effect to invert the color of a pixel. This preserves the alpha component.
    class ColorInversion < Effect
      effect_name :color_inversion

      def code
        return <<code
vec4 do_color_inversion(ray_color_inversion args, vec4 color) {
  return vec4(vec3(1, 1, 1) - color.rgb, color.a);
}
code
      end
    end
  end
end
