require File.expand_path(File.dirname(__FILE__)) + '/helpers.rb'

class RandomAnimation < Ray::Animation
  register_for :random_animation

  def setup
    @setup_called = true
  end

  def setup_target
  end

  def update_target
  end

  def setup_called?; @setup_called; end
end

context "an animation" do
  setup do
    Ray::Animation.new
  end

  denies :running?
  asserts(:duration).equals 0
  asserts(:target).nil
  asserts(:progression).nil

  context "running for 10 seconds" do
    hookup do
      topic.duration = 10
      topic.start(@target = Object.new)
    end

    asserts :running?
    asserts(:duration).equals 10
    asserts(:target).equals { @target }

    context "paused" do
      hookup { topic.pause }

      asserts :paused?
      denies  :running?

      context "resumed" do
        hookup { topic.resume }

        denies  :paused?
        asserts :running?
      end
    end
  end
end

context "a random animation" do
  setup do
    obj = Object.new
    obj.extend Ray::Helper
    obj.random_animation
  end

  asserts :setup_called?

  context "started" do
    hookup do
      stub(topic).setup_target
      topic.start @target = Object.new
    end

    asserts_topic.received :setup_target

    context "updated" do
      hookup do
        stub(topic).update_target
        topic.update
      end

      asserts_topic.received :update_target
    end
  end
end

run_tests if __FILE__ == $0
