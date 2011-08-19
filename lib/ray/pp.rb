module Ray
  # Module containing helpers to implement pretty print
  module PP
    # @param [PrettyPrint] q Object used for pretty printing
    # @param [Array<String>] attributes Methods to call and to print the result
    #   of
    def pretty_print_attributes(q, attributes)
      id = "%x" % (__id__ * 2)
      id.sub!(/\Af(?=[[:xdigit:]]{2}+\z)/, '') if id.sub!(/\A\.\./, '')

      klass = self.class.pretty_inspect.chomp

      q.group(2, "\#<#{klass}:0x#{id}", '>') do
        q.seplist(attributes, lambda { q.text ',' }) do |key|
          q.breakable

          q.text key.to_s
          q.text '='

          q.group(2) do
            q.breakable ''
            q.pp send(key)
          end
        end
      end
    end
  end
end
