module Ray
  module TextHelper
    module_function

    if defined? ::Encoding
      # @param [String] string Any string
      # @param [String] enc Name of the encoding of String. Gueseed in 1.9.
      #
      # @return [String] String in Ray's internal encoding
      def internal_string(string, enc = "UTF-8")
        string.encode(InternalEncoding)
      end

      # @param [String] string A string encoded using Ray's internal encoding
      # @param [String] enc Output encoding
      #
      # @return [String] string converted to the asked encoding
      def convert(string, enc = "UTF-8")
        string.encode enc
      end
    else
      def internal_string(string, enc = "UTF-8")
        Iconv.conv(InternalEncoding, enc, string)
      end

      def convert(string, enc = "UTF-8")
        Iconv.conv(enc, InternalEncoding, string)
      end
    end
  end
end
