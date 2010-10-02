module Ray
  class Font
    extend Ray::ResourceSet
    need_argument_count 1
    add_set(/^(.*)$/) { |filename, size| new(filename, size) }

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

class String
  # Draws the receiver.
  #
  # @option opts [Ray::Font] :font The font used to render the string.
  # @option opts [Ray::Image] :on The image to draw on.
  # @option opts [Integer] :w (:width) and :h (:height) Size of the image
  #                        if :on isn't specified. :w is required in that case.
  # @option opts [Symbol] The encoding. Can be guessed in Ruby 1.9.
  # @option opts [Ray::Color] :color The color to draw the text in.
  # @option opts [Ray::Color] :background Background color in shaded mode.
  # @option opts [Symbol] :mode The drawing mode.
  # @option opts [Array<Symbol>] :style The different styles to apply.
  #                                     :italic, :bold, and :underlined.
  #
  # @option opts [Array<Integer>] :at Where the image should be drawn.
  #                                   Defaults to (0, 0)
  # @see Ray::Font#draw
  def draw(opts = {})
    font = opts[:font]

    lines = split(/\r\n|\n|\r/)
    line_skip = font.line_skip

    target = opts[:on]

    string_encoding = opts[:encoding]
    string_encoding ||= if respond_to? :encoding # Ruby 1.9
                          case encoding.to_s
                          when /^utf-?8$/i
                            :utf8
                          when /^iso-8859-/i
                            :latin1
                          else
                            nil
                          end
                        else
                          nil
                        end

    target ||= Ray::Image.new(:height => opts[:height] || opts[:h] ||
                              line_skip * lines.size,
                              :width  => opts[:width] || opts[:w] ||
                              lines.map { |i|
                                font.size_of(self, string_encoding).width
                              }.max)

    color      = opts[:color]
    background = opts[:background]

    mode = opts[:mode]

    if styles = opts[:style]
      font.style = styles.inject(0) do |flags, style|
        flags |= case style
                 when :italic
                   Ray::Font::STYLE_ITALIC
                 when :bold
                   Ray::Font::STYLE_BOLD
                 when :underlined
                   Ray::Font::STYLE_UNDERLINE
                 else
                   raise "Unknown flag #{style}"
                 end
      end
    end

    x, y = opts[:at]
    x ||= 0
    y ||= 0

    lines.each do |line|
      font.draw(line, :on => target, :at => [x, y], :encoding => string_encoding,
                :color => color, :background => background, :mode => mode)
      y += line_skip
    end

    target
  end
end
