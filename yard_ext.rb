require 'yard'

class MatcherHandler < YARD::Handlers::Ruby::Base
  handles method_call(:describe_matcher)

  def process
    src = statement.parameters.children.first.source[1..-1]
    MethodObject.new(P("Ray::Matchers"), src) do |o|
      register(o)

      args = statement.last.first
      if args
        o.parameters = [args.source]
      end
    end
  end
end

class CustomCParser < YARD::Parser::CParser
  include YARD

  def parse
    super
    parse_constants
  end

  private

  def handle_constants(type, var_name, const_name, definition)
    namespace = @namespaces[var_name]
    obj = CodeObjects::ConstantObject.new(namespace, const_name)
    obj.value = definition
  end

  def parse_constants
    @content.scan(%r{\Wrb_define_
                   ( variable          |
                     readonly_variable |
                     const             |
                     global_const      | )
               \s*\(
                 (?:\s*(\w+),)?
                 \s*"(\w+)",
                 \s*(.*?)\s*\)\s*;
                 }xm) do |type, var_name, const_name, definition|
      var_name = "rb_cObject" if !var_name or var_name == "rb_mKernel"
      handle_constants type, var_name, const_name, definition
    end
  end
end

YARD::Parser::SourceParser.register_parser_type(:c, CustomCParser,
                                                ['c', 'cc', 'cxx', 'cpp'])
