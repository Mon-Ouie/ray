module Ray
  class Font
    # @return [true, false] True if the font has no style
    def normal?
      style == STYLE_NORMAL
    end

    # @return [true, false] True if the font is italic
    def italic?
      (style & STYLE_ITALIC) != 0
    end

    #  @return [true, false] True if the font is bold
    def bold?
      (style & STYLE_BOLD) != 0
    end

    # @return [true, false] True if the font is underlined
    def underlined?
      (style & STYLE_UNDERLINE) != 0
    end
  end
end
