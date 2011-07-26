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

  private
  def find_method_body(object, func_name, content = @content)
    case content
    when %r"((?>/\*.*?\*/\s*))(?:(?:static|SWIGINTERN)\s+)?(?:intern\s+)?
            VALUE\s+#{func_name}\s*(\([^)]*\))([^;]|$)"xm
      comment, params = $1, $2
      body_text = $&

      remove_private_comments(comment) if comment

      # see if we can find the whole body

      re = Regexp.escape(body_text.strip) + '[^(]*\{.*?^\}'
      if /#{re}/m =~ content
        body_text = $&
      end

      # The comment block may have been overridden with a 'Document-method'
      # block. This happens in the interpreter when multiple methods are
      # vectored through to the same C method but those methods are logically
      # distinct (for example Kernel.hash and Kernel.object_id share the same
      # implementation

      # override_comment = find_override_comment(object)
      # comment = override_comment if override_comment

      object.docstring = parse_comments(object, comment) if comment
      object.source = body_text
    when %r{((?>/\*.*?\*/\s*))^\s*\#\s*define\s+#{func_name}\s+(\w+)}m
      comment = $1
      find_method_body(object, $2, content)
    else
      # No body, but might still have an override comment
      # comment = find_override_comment(object)
      comment = nil
      object.docstring = parse_comments(object, comment) if comment
    end
  end

  MethodRegexp = %r{rb_define_
                       (
                          singleton_method |
                          method           |
                          module_function  |
                          private_method
                       )
                       \s*\(\s*([\w\.]+),
                         \s*"([^"]+)\",
                         \s*(?:RUBY_METHOD_FUNC\(|VALUEFUNC\()?(\w+)\)?,
                         \s*(-?\w+)\s*\)
                       (?:;\s*/[*/]\s+in\s+(\w+?\.[cy]))?
                     }xm

  def parse_methods
    id = 0
    while next_id = @content.index(MethodRegexp, id)
      id = next_id + 1

      type, var_name, name, func_name, param_count, source_file = $~.captures

      next if var_name == "ruby_top_self"
      next if var_name == "nstr"
      next if var_name == "envtbl"

      end_group   = @content.rindex(/@endgroup/, id)
      begin_group = @content.rindex(/@group (.*?)(?:\s*\*\/)?$/, id)

      if (begin_group and not end_group) or
          (begin_group and end_group and begin_group > end_group)
        group = $1
      else
        group = nil
      end

      var_name = "rb_cObject" if var_name == "rb_mKernel"
      handle_method(type, var_name, name, func_name, source_file, group)
    end

    # Damn that warning about shadowing, or rather the fact variables defined in
    # a loop are preserved.
    @content.scan(%r{rb_define_global_function\s*\(
                               \s*"([^"]+)\",
                               \s*(?:RUBY_METHOD_FUNC\(|VALUEFUNC\()?(\w+)\)?,
                               \s*(-?\w+)\s*\)
                 (?:;\s*/[*/]\s+in\s+(\w+?\.[cy]))?
                 }xm) do |name, func_name, param_count, source_file|
      handle_method("method", "rb_mKernel", name, func_name, source_file)
    end
  end

  def handle_method(scope, var_name, name, func_name, source_file = nil,
                    group = nil)

    # Don't consider module functions as class methods to be consistent with how
    # they are interpreted by the Ruby parser.
    scope = (scope == "singleton_method") ? :class : :instance

    namespace = @namespaces[var_name] || P(remove_var_prefix(var_name))
    ensure_loaded!(namespace)
    obj = CodeObjects::MethodObject.new(namespace, name, scope)
    obj.add_file(@file)
    obj.parameters = []
    obj.docstring.add_tag(YARD::Tags::Tag.new(:return, '', 'Boolean')) if name =~ /\?$/
    obj.source_type = :c
    obj.group = group

    namespace.groups << group if group and not namespace.groups.include?(group)

    content = nil
    begin
      content = File.read(source_file) if source_file
    rescue Errno::ENOENT
      path = "#{namespace}#{scope == :instance ? '#' : '.'}#{name}"
      log.warn "Missing source file `#{source_file}' when parsing #{path}"
    ensure
      content ||= @content
    end
    find_method_body(obj, func_name, content)
  end
end

YARD::Parser::SourceParser.register_parser_type(:c, CustomCParser,
                                                ['c', 'cc', 'cxx', 'cpp'])
