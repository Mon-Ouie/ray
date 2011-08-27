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
    case actual
    when Numeric
      (actual - expected).abs <= epsilon
    when Ray::Vector2, Ray::Vector3
      (actual - expected).to_a.all? { |n| n.abs <= epsilon }
    when Ray::Matrix
      actual.content.zip(expected.content).all? do |a, b|
        (a - b).abs <= epsilon
      end
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
