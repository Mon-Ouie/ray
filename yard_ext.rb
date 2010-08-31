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

class TypeHandler < YARD::Handlers::Ruby::Base
  handles method_call(:register_type)

  def process
    type_name = statement.parameters.children.first.source
    klass     = statement.parameters.children.last.source

    obj = YARD::Registry.resolve(P("Ray"), "resolve_type")
    return unless obj

    obj[:registred_types] ||= []
    obj[:registred_types] << [type_name, klass]
  end
end

class ConversionHandler < YARD::Handlers::Ruby::Base
  handles method_call(:describe_conversion)

  def process
    args = statement.parameters.children.first.source.split(" => ")

    obj = YARD::Registry.resolve(P("Ray"), "convert")
    return unless obj

    obj[:converters] ||= []
    obj[:converters] << args
  end
end
