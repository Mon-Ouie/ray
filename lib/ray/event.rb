module Ray
  class Event
    if defined? Encoding
      alias text_without_encoding text

      # @return [String, nil] The entered text in internal encoding
      def text
        if content = text_without_encoding
          content.force_encoding(InternalEncoding)
        end
      end
    else
      # @param [String, Encoding] encoding The encoding to be used.
      # @return [String, nil] The entered text using the provided encoding.
      def text_as(encoding)
        if content = text
          Ray::TextHelper.convert(content, encoding)
        end
      end
    end
  end
end
