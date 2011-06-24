require File.expand_path(File.dirname(__FILE__)) + '/helpers.rb'

context "a scene list" do
  setup do
    @game = a_small_game
    10.times do |i|
      @game.scene "scene_#{i}".to_sym do
        always { exit }
      end
    end

    Ray::SceneList.new @game
  end

  asserts :empty?
  asserts(:current).nil
  asserts(:to_a).equals []

  denies("exiting from the current scene") {
    topic.exit_current
  }.raises_kind_of Exception

  asserts("adding an unknown scene to the list") {
    topic << :an_awesome_scene_that_doesnt_exist
  }.raises_kind_of ArgumentError

  context "with two scenes" do
    hookup do
      topic.push :scene_0, 0
      topic.push :scene_5, 5
    end

    denies :empty?

    asserts(:to_a).equals {
      [@game.registered_scene(:scene_0), @game.registered_scene(:scene_5)]
    }

    asserts(:current).equals { @game.registered_scene(:scene_5) }

    context "running the current scene" do
      hookup do
        stub(topic.current).run
        topic.run_current
      end

      asserts(:current).equals { @game.registered_scene(:scene_5) }
      asserts(:current).received :run

      asserts("current scene's arguments") {
        topic.current.scene_arguments
      }.equals [5]
    end

    context "after #pop" do
      hookup { topic.pop }

      denies :empty?

      asserts(:to_a).equals { [@game.registered_scene(:scene_0)] }
      asserts(:current).equals { @game.registered_scene(:scene_0) }
    end

    context "after #clear" do
      hookup { topic.clear }

      asserts :empty?

      asserts(:to_a).equals []
      asserts(:current).nil

      asserts("running the current scene") {
        topic.run_current
      }.raises_kind_of Exception
    end
  end
end

run_tests if __FILE__ == $0
