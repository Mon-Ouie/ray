$:.unshift File.expand_path(File.join(File.dirname(__FILE__), "..", "ext"))
$:.unshift File.expand_path(File.join(File.dirname(__FILE__), "..", "lib"))

require 'riot'
require 'rr'

require 'ray'

require 'ostruct'
require 'stringio'

include Math

def path_of(res)
  File.expand_path(File.join(File.dirname(__FILE__), "res", res))
end

def run_tests
  exit Riot.run.success?
end

def deg_to_rad(deg)
  deg * PI / 180
end

class AlmostEqualMacro < Riot::AssertionMacro
  register :almost_equals

  def test(actual, expected, epsilon)
    diff = (actual - expected)
    case diff
    when Numeric
      diff.abs <= epsilon
    when Ray::Vector2, Ray::Vector3
      diff.to_a.all? { |n| n.abs <= epsilon }
    end
  end

  def evaluate(actual, expected, epsilon = Float::EPSILON)
    if test(actual, expected, epsilon)
      pass "is almost equal to #{expected}"
    else
      fail "#{actual} +/- #{epsilon} is not equal to #{expected}"
    end
  end

  def devaluate(actual, expected, epsilon = Float::EPSILON)
    if test(actual, expected, epsilon)
      fail "#{actual} is almost equal to #{expected}"
    else
      pass "+/- #{epsilon} is almost equal to #{expected}"
    end
  end
end

class ReceivedMacro < Riot::AssertionMacro
  register :received

  include RR::Adapters::RRMethods

  def evaluate(recv, msg, *args)
    msg, times = case msg
                 when Symbol then [msg, 1]
                 when Hash   then msg.each.first
                 end

    begin
      received(recv).__send__(msg, *args).times(times).call
      pass "received ##{msg} with #{args.map { |o| o.inspect }.join(', ') } " +
        "#{times} time#{'s' if times != 1}"
    rescue ::RR::Errors::RRError => e
      fail "#{recv.inspect} didn't receive ##{msg} with " +
        "#{args.map { |o| o.inspect }.join(', ') } " +
        "#{times} time#{'s' if times != 1}"
    end
  end

  def devaluate(recv, msg, *args)
    msg, times = case msg
                 when Symbol then [msg, 1]
                 when Hash   then msg.each.first
                 end

    begin
      received(recv).__send__(msg, *args).times(times).call
      fail "#{recv.inspect} received ##{msg} with " +
        "#{args.map { |o| o.inspect }.join(', ') } " +
        "#{times} time#{'s' if times != 1}"
    rescue ::RR::Errors::RRError => e
      pass "received ##{msg} with " +
        "#{args.map { |o| o.inspect }.join(', ') } " +
        "#{times} time#{'s' if times != 1}"
    end
  end
end

module Riot
  class RaisesKindOfMacro < AssertionMacro
    register :raises_kind_of
    expects_exception!

    def evaluate(actual_exception, expected_class, expected_message=nil)
      actual_message = actual_exception && actual_exception.message

      if !actual_exception
        fail new_message.expected_to_raise_kind_of(expected_class).but.raised_nothing
      elsif !actual_exception.is_a?(expected_class)
        fail new_message.expected_to_raise_kind_of(expected_class).
          not(actual_exception.class)
      elsif expected_message && !(actual_message.to_s =~ %r[#{expected_message}])
        fail expected_message(expected_message).for_message.not(actual_message)
      else
        message = new_message.raises_kind_of(expected_class)
        pass(expected_message ? message.with_message(expected_message) : message)
      end
    end

    def devaluate(actual_exception, expected_class, expected_message=nil)
      actual_message = actual_exception && actual_exception.message

      if !actual_exception
        pass new_message.raises_kind_of(expected_class)
      elsif !actual_exception.is_a?(expected_class)
        if expected_message && !(actual_message.to_s =~ %r[#{expected_message}])
          pass new_message.raises_kind_of(expected_class).
            with_message(expected_message)
        else
          pass new_message.raises_kind_of(expected_class)
        end
      else
        message = new_message.expected_to_not_raise_kind_of(expected_class)

        if expected_message
          fail message.with_message(expected_message).but.
            raised(actual_exception.class).
            with_message(actual_exception.message)
        else
          fail message
        end
      end
    end
  end unless defined? RaisesKindOfMacro
end


class Riot::Context
  include RR::Adapters::RRMethods
end

class Riot::Situation
  include RR::Adapters::RRMethods
end

def animation_context(name, method, opts, &block)
  obj = Object.new
  obj.extend Ray::Helper

  context name do
    helper(:animation) { obj.send(method, opts.merge(:duration => 5)) }
    helper(:finished_animation) { obj.send(method, opts.merge(:duration => 0)) }
    helper(:reversed_animation) { -obj.send(method, opts.merge(:duration => 5)) }

    instance_eval(&block)
  end
end

class Object
  win = nil

  define_method :a_small_game do |*args|
    name = args.first || "test"

    unless win
      win = Ray::Window.new
      win.open name, [640, 480]
    end

    Ray::Game.new(name, :window => win)
  end
end

Riot.reporter = Riot::PrettyDotMatrixReporter unless ARGV.include? "--verbose"
Riot.alone!
