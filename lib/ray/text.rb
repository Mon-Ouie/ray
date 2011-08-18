module Ray
  class Text < Drawable
    include TextHelper

    # @param [String] string The content of the text
    # @option opts :encoding ("utf-8") The encoding for the text. Unneeded in
    #   1.9.
    # @option opts :size (12) The character size.
    # @option opts :style (Font::STYLE_NORMAL) Style of the text.
    # @option opts :at ((0, 0)) Position of the text.
    # @option opts :scale ((1, 1)) Scale applied to the text.
    # @option opts :zoom Same as :scale
    # @option opts :angle (0) Rotation applied to the text.
    # @option opts :color (Ray::Color.white) The color used to draw the text
    # @option opts :font [Ray::Font, String] (Ray::Font.default) Font used to draw
    # @option opts :shader [Ray::Shader] (nil) Shader
    def initialize(string, opts = {})
      opts = {
        :encoding => string.respond_to?(:encoding) ? string.encoding : "utf-8",
        :size     => 12,
        :style    => Normal,
        :at       => [0, 0],
        :angle    => 0,
        :color    => Ray::Color.white
      }.merge(opts)

      @encoding = opts[:encoding].to_s

      self.string = string
      self.size   = opts[:size]
      self.style  = opts[:style]
      self.pos    = opts[:at]
      self.scale  = opts[:scale] || opts[:zoom] || [1, 1]
      self.angle  = opts[:angle]
      self.color  = opts[:color]
      self.shader = opts[:shader]

      if font = opts[:font]
        self.font = font.is_a?(String) ? Ray::FontSet[font] : font
      end
    end

    # @param [Integer, Array<Symbol>] style Flags for the font style.
    #  Valid symbols are :normal, :italic, :bold, and :underline.
    def style=(style)
      set_basic_style parse_style(style)
    end

    # Sets the string used by the text.
    # In 1.9, @encoding is changed approprietly. In 1.8, it is defaulted
    # to utf-8. Change it to whatever is the right encoding.
    # @return [String] The string used by the text, using @encoding as the
    #   encoding.
    def string
      str = basic_string
      if str.respond_to? :force_encoding
        str.force_encoding InternalEncoding
      end

      convert(str, @encoding)
    end

    # Sets the string used by the text.
    # In 1.9, @encoding is changed approprietly. In 1.8, it is defaulted
    # to utf-8. Change it to whatever is the right encoding.
    def string=(val)
      if val.respond_to? :encoding
        @encoding = val.encoding.to_s
      end

      set_basic_string internal_string(val, @encoding)
    end

    # (see Rect#collide?)
    def collide?(obj)
      rect.collide?(obj.to_rect)
    end

    # (see Rect#inside?)
    def inside?(obj)
      rect.inside?(obj.to_rect)
    end

    # (see Rect#outside?)
    def outside?(obj)
      rect.outside?(obj.to_rect)
    end

    alias :to_rect :rect

    # @return [String]
    attr_accessor :encoding

    def inspect
      "text(#{string.inspect})"
    end

    alias :to_s :string

    def pretty_print(q)
      attr = %w[
        string
        font color size style

        rect auto_center
      ]

      pretty_print_attributes q, attr
    end

    private
    def parse_style(style)
      case style
      when Integer
        style
      when Array
        style.inject(0) do |last, e|
          last | case e
                 when :normal     then Normal
                 when :italic     then Italic
                 when :bold       then Bold
                 when :underlined then Underlined
                 else
                   raise ArgumentError, "Unknown style #{e.inspect}"
                 end
        end
      when nil
        Normal
      else
        raise ArgumentError, "Can't convert #{style.class} into Integer"
      end
    end
  end
end
