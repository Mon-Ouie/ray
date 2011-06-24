require File.expand_path(File.dirname(__FILE__)) + '/helpers.rb'

context "an event runner" do
  helper(:register_for) do |name, *args|
    stub(@stub).__send__(name)
    @obj.on(name, *args) { |*block_args| @stub.__send__(name, *block_args) }
  end

  helper(:raise_event)   { |name, *args| @obj.raise_event(name, *args) }

  helper(:enable_group)  { |name| @obj.enable_event_group(name) }
  helper(:disable_group) { |name| @obj.disable_event_group(name) }

  helper(:event_group)   { |name| @obj.current_event_group = name }

  def runner_asserts(name, method = :asserts, &block)
    send(method, "after #{name}") do
      reset

      [:default, :test].each { |o| @obj.remove_event_group o }
      instance_eval(&block)
      topic.run

      @stub
    end
  end

  def runner_denies(name, &block)
    runner_asserts(name, :denies, &block)
  end

  setup do
    runner = Ray::DSL::EventRunner.new

    @stub = Object.new
    class << @stub
      def inspect; "stub"; end
    end

    @obj = Object.new
    @obj.extend Ray::Helper
    @obj.event_runner = runner

    runner
  end

  runner_asserts("sending a registered event") {
    register_for :event
    raise_event :event
  }.received(:event)

  runner_denies("sending an unregistered event") {
    register_for :foo
    raise_event :event
  }.received(:event)

  runner_asserts("sending a registered event with the same arguments") {
    register_for :event, "argument"
    raise_event :event, "argument"
  }.received(:event, "argument")

  runner_asserts("sending a registered event with matching arguments") {
    register_for :event, /arg/
    raise_event :event, "argument"
  }.received(:event, "argument")

  runner_denies("sending a registered event with less arguments") {
    register_for :event, "argument1", "argument2"
    raise_event :event, "argument1"
  }.received(:event, "argument1")

  runner_asserts("sending a register event with more arguments") {
    register_for :event
    raise_event :event, "bar"
  }.received(:event, "bar")

  runner_denies("sending an event registered in a disabled event group") {
    event_group   :test
    register_for  :event
    disable_group :test

    raise_event :event
  }.received(:event)

  runner_asserts("sending an event registered in an enabled event_group") {
    event_group   :test
    register_for  :event
    disable_group :test
    enable_group  :test

    raise_event :event
  }.received(:event)
end

run_tests if __FILE__ == $0
